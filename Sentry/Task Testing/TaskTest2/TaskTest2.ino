// SEEMS Demo Video Code.
// This code was written to demonstrate the integration of 3 Major components of the System:
// 1. 2 TT Motors
// 2. 1 MCU
// 3. 1 HC-SR04
// 4. 1 KY-037 using CMA-6542PF

// Mic Pin.
#define MIC_ANALOG 8

// Motor Pins.
#define R_MOT_1 4
#define R_MOT_2 5
#define L_MOT_1 6
#define L_MOT_2 7

// Ultrasonic Sensor Pins.
#define ECHO 9
#define TRIGGER 10

// LEDC PWM Stuff;
#define FREQ 20000
#define RES 8
#define LED_C_LOW 0
#define LED_C_HIGH 255

// ESP32-S3-Devkitc-1 on board rgb led.
#define RGB_PIN 38      

// Serial monitor baud rate.
#define BAUD_RATE 9600

// Sensor thresholds.
const int soundThreshold = 50;          // 50 dB for mic based on clap testing.
const int distanceThreshold = 5.90;    // ~6 in. for ultrasonic sensor. Aribtrary decision.

// Tasks and task handles.
void pollKY037(void *pvParameter);
void pollHCSR04(void *pvParameter);
TaskHandle_t hearSound = NULL;
TaskHandle_t measureDistance = NULL;

// Threshold detection register.
// 0000_0000  ->  No threshold breach detected.
// 1111_0000  ->  Sound level breah deteced.
// 0000_1111  ->  Distance level breach detected.
unsigned char thresholdFlag = 0;     
#define SOUND_BREACH      0xF0
#define DISTANCE_BREACH   0x0F

void setup() {

  // Setup peripherals.
  setupMotors();
  setupUltrasonic();
  setupMicrophone();

  ///*
  // Declare task for polling the mic.
  xTaskCreatePinnedToCore(
    &pollKY037,       // Task address
    "pollKY037",      // Task name
    2000,             // Number of bytes to reserve for the task
    NULL,             // Parameters to reserve for the task
    1,                // Priority of the task
    &hearSound,       // Handle to the task
    0                 // Core to select
  );

  // Declare task for polling the ultrasonic sensor.
  xTaskCreatePinnedToCore(
    &pollHCSR04,        // Task address
    "pollHCSR04",       // Task name
    4096,               // Number of bytes to reserve for the task
    NULL,               // Parameters to reserve for the task
    2,                  // Priority of the task
    &measureDistance,   // Handle to the task
    1                   // Core to select
  );

  // Suspend distance monitoring until it's needed.
  vTaskSuspend(measureDistance);
  //*/
  // Begin serial output.
  Serial.begin(BAUD_RATE);
  Serial.println("Setup went well");
}

// Loop remains empty.
void loop() {}


/********* Tasks *********/

// Task to take the output of the micropone and start motors on threshold breach.
void pollKY037(void *pvParameter) {

  for(;;) {
    // Read microphone output.
    int audioOutputData = readMicrophone();

    // If necessary: raise flag, move sentry forward. 
    Serial.printf("Ears are working\n");
    if(audioOutputData > soundThreshold) {
      // Debug output.
      Serial.printf("Loud Noise Detected: Moving Sentry Forward.\n");

      thresholdFlag = SOUND_BREACH;
      sentryForward(L_MOT_1, L_MOT_2, R_MOT_1, R_MOT_2);

      // Resume distance monitoring and suspend this task until it's needed again.
      vTaskResume(measureDistance);
      vTaskSuspend(NULL);       
    }
    Serial.printf("pollHCSR04 stack usage: %d\n", uxTaskGetStackHighWaterMark(NULL));
    vTaskDelay(100/portTICK_RATE_MS);
  }
}

// Task to take the output of the ultrasonic sensor and stop motors on threshold breach.
void pollHCSR04(void *pvParameter) {

  for(;;) {
    // Do not begin monitoring until sound threshold has been breached.
    if(thresholdFlag != SOUND_BREACH) vTaskDelay(100/portTICK_RATE_MS);;

    // Read current Sentry measured distance in inches.
    int currDistance = readUltrasonicSensor();
    
    // If necessary: raise flag, stop sentry.
    if(currDistance < distanceThreshold) {
      // Debug output.
      Serial.printf("Obstacle Detected: Stopping Sentry Movement.\n");

      thresholdFlag = DISTANCE_BREACH;
      sentryBrake();

      // Resume noise detection and suspend this task until it's needed again.
      vTaskResume(hearSound);
      vTaskSuspend(NULL);
    }
    vTaskDelay(100/portTICK_RATE_MS);
  }
}

/********* Motor Functions *********/

// Function to setup motors for output.
void setupMotors() {
  // Set pins as LED  PWM Output.
  ledcAttach(L_MOT_1, FREQ, RES);
  ledcAttach(L_MOT_2, FREQ, RES);
  ledcAttach(R_MOT_1, FREQ, RES);
  ledcAttach(R_MOT_2, FREQ, RES);

  // Initialize pins as low. Prevent unintended startup.
  ledcWrite(L_MOT_1, LED_C_LOW);
  ledcWrite(L_MOT_2, LED_C_LOW);
  ledcWrite(R_MOT_1, LED_C_LOW);
  ledcWrite(R_MOT_2, LED_C_LOW);
}

// Function to make a motor turn clockwise.
void motorCW(int in1, int in2) {
  ledcWrite(in1, LED_C_LOW);
  ledcWrite(in2, LED_C_HIGH);
}

// Function to make a motor turn counter clockwise.
void motorCCW(int in1, int in2) {
  ledcWrite(in1, LED_C_HIGH);
  ledcWrite(in2, LED_C_LOW);
}

// Function to turn a Motor off.
void motorStop(int in1, int in2) {
  ledcWrite(in1, LED_C_LOW);
  ledcWrite(in2, LED_C_LOW);
}

// Function to move Sentry forwards.
void sentryForward(int in1_L, int in2_L, int in1_R, int in2_R) {
  motorCCW(in1_L, in2_L);
  motorCW(in1_R, in2_R);
}

// Function to stop Sentry movement.
void sentryBrake() {
  motorStop(L_MOT_1, L_MOT_2);
  motorStop(R_MOT_1, R_MOT_2);
}


/********* Ultrasonic Sensor Functions *********/

// Function to setup ultrasonic sensor.
void setupUltrasonic() {
  // Configure sensor pins.
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

// Function to read value from ultrasonic sensor.
float readUltrasonicSensor() {
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);

  // Read the signal from the sensor and convert time into distance traveled.
  float duration = pulseIn(ECHO, HIGH);
  float inches = (duration / 2) / 74; // Divide by 74 or multiply by 0.0135.

  // Return.
  Serial.printf("Curr distance: %f\n", inches);
  return inches;
}

/********* Microphone Functions *********/

// Function to setup microphone for input.
void setupMicrophone() {
  pinMode(MIC_ANALOG, INPUT);
}

// Function to read microphone analog output and transform into dB.
int readMicrophone() {
  const unsigned long startTime = millis();
  const unsigned long sampleWindow = 50;
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 4095;
  
  // Sample the audio signal.
  int sample = 0;
  while(millis() - startTime < sampleWindow) {
    sample = 4095 - analogRead(MIC_ANALOG);
    if(sample < 4095) {
      if(sample > signalMax) signalMax = sample;
      else if (sample < signalMin) signalMin = sample;
    }
  }

  // Remap analog values to decibel range (improper).
  peakToPeak = signalMax - signalMin;
  int dB = map(peakToPeak, 0, 4095, 34, 120);

  // Return sound measurement.
  return dB;
}

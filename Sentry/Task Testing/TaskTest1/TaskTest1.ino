// Serial monitor baud rate.
#define BAUD_RATE 9600

// Tasks and task handles.
void pollKY037(void *pvParameter);
void pollHCSR04(void *pvParameter);
TaskHandle_t hearSound = NULL;
TaskHandle_t measureDistance = NULL;

void setup() {
  // Begin serial output.
  Serial.begin(BAUD_RATE);

  // Declare task for polling the mic.
  BaseType_t micTask = xTaskCreatePinnedToCore(
    &pollKY037,       // Task address
    "pollKY037",      // Task name
    2000,             // Number of bytes to reserve for the task
    NULL,             // Parameters to reserve for the task
    1,                // Priority of the task
    &hearSound,       // Handle to the task
    0                 // Core to select
  );

  if(micTask != pdPASS) Serial.println("Ya goofed 1.");

  // Declare task for polling the ultrasonic sensor.
  BaseType_t obsTask = xTaskCreatePinnedToCore(
    &pollHCSR04,        // Task address
    "pollHCSR04",       // Task name
    2000,               // Number of bytes to reserve for the task
    NULL,               // Parameters to reserve for the task
    2,                  // Priority of the task
    &measureDistance,   // Handle to the task
    1                   // Core to select
  );

  if(obsTask != pdPASS) Serial.println("Ya goofed 2.");
}

// Loop remains empty.
void loop() {}


/********* Tasks *********/

// Task to take the output of the micropone and start motors on threshold breach.
void pollKY037(void *pvParameter) {
 
  // If necessary: raise flag, move sentry forward. 
  for(;;) {
    Serial.printf("Ears are working\n");
    vTaskDelay(10 / portTICK_RATE_MS);
  }
}

// Task to take the output of the ultrasonic sensor and stop motors on threshold breach.
void pollHCSR04(void *pvParameter) {
 
  // Debug output.
  for(;;) {
  Serial.printf("Obstacle Detected: Stopping Sentry Movement.\n");
  vTaskDelay(10 / portTICK_RATE_MS);
  }
}


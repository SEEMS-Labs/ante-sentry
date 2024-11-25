
#define MIC_ANALOG 8          // KY-038 Analog Output
#define RGB_PIN 38            // Built in RGB Led
#define BAUD_RATE 9600        // Serial monitor baud rate.


void setup() {
  pinMode(MIC_ANALOG, INPUT);       // Set the MIC pin as input
  pinMode(RGB_PIN, OUTPUT);         // Set the LED pin as output
  digitalWrite(RGB_PIN, LOW);       // Set the LED pin as output
  Serial.begin(BAUD_RATE);          // Initialize serial communication

}

const int sampleWindow = 50;
unsigned int sample;
unsigned int clapSoundLevel = 0;    // measured clap sound level.

void loop() {
  /*
  float micValue = analogRead(MIC_ANALOG);           // Read digital value from the microphone
  Serial.println(micValue);                     // Print the digital value to Serial Monitor

  // Turn the LED on if sound is detected (micValue HIGH)
  if (micValue == HIGH) {
    neopixelWrite(RGB_PIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS);  // Turn the LED on
    delay(500);
  } else {
    neopixelWrite(RGB_PIN, 0, 0, 0);   // Turn the LED off
  }
  delay(100);
  */

  const unsigned long startTime = millis();
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 4095;
  
  while(millis() - startTime < sampleWindow) {
    //unsigned long delta = millis() - startTime;
    //Serial.printf("delta %ld\n", delta);
    sample = 4095 - analogRead(MIC_ANALOG);
    //Serial.println(sample);
    if(sample < 4095) {
      if(sample > signalMax) signalMax = sample;
      else if (sample < signalMin) signalMin = sample;
    }
  }

  // Remap to decibel.
  peakToPeak = signalMax - signalMin;
  int db = map(peakToPeak, 0, 4095, 34, 120);

  // Measure clap sound level.
  if(db > clapSoundLevel) {
    clapSoundLevel = db;
    Serial.printf("New Clap Sound Level: %d dB\n", clapSoundLevel);
  }
}











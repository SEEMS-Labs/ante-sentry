#define MIC_DIGITAL_PIN 35  // KY-038 Digital Output pin connected to GPIO35
#define LED_PIN 5           // External LED connected to GPIO4
int micValue;
void setup() {
  pinMode(MIC_DIGITAL_PIN, INPUT);  // Set the MIC pin as input
  pinMode(LED_PIN, OUTPUT);         // Set the LED pin as output
  digitalWrite(MIC_DIGITAL_PIN, LOW);  // Set the MIC pin as input
  digitalWrite(LED_PIN, LOW);         // Set the LED pin as output
  Serial.begin(115200);             // Initialize serial communication
}

void loop() {
  micValue = digitalRead(MIC_DIGITAL_PIN);  // Read digital value from the microphone
  Serial.println(micValue);                     // Print the digital value to Serial Monitor

  // Turn the LED on if sound is detected (micValue HIGH)
  if (micValue == HIGH) {
    digitalWrite(LED_PIN, HIGH);  // Turn the LED on
    delay(500);
  } else {
    digitalWrite(LED_PIN, LOW);   // Turn the LED off
  }

}
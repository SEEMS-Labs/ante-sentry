#define RGB_PIN 38

void setup() {
  // No need to initialize the RGB LED
  //Serial.begin(115200);
  pinMode(RGB_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  Serial.printf(".\n");
#ifdef RGB_PIN
  neopixelWrite(RGB_PIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS);  // Turn the RGB LED white
  delay(1000);
  neopixelWrite(RGB_PIN, 0, 0, 0);  // Turn the RGB LED off
  delay(1000);

  neopixelWrite(RGB_PIN, RGB_BRIGHTNESS, 0, 0);  // Red
  delay(1000);
  neopixelWrite(RGB_PIN, 0, RGB_BRIGHTNESS, 0);  // Green
  delay(1000);
  neopixelWrite(RGB_PIN, 0, 0, RGB_BRIGHTNESS);  // Blue
  delay(1000);
  neopixelWrite(RGB_PIN, 0, 0, 0);  // Off / black
  delay(1000);
#endif
}

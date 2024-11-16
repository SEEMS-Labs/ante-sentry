// Sensor Pins:
// Vcc = 5V
// Trig = 14
// Echo = 12
// Gnd = Gnd
#include <Arduino.h>
#include <LiquidCrystal.h>
#define BAUD_RATE 9600
#define DELAY_LEN 250 * 4

// Ultrasonic Sensor Pin Definitions.
#define ECHO 12
#define TRIGGER 14

// Ultrasonic sensor variables.
double duration, cm, inches;
char str[100];

// Ultrasonic sensor Functions.
void initSerialOutput();
void initSensor();
void readSensor();

// Setup code. Runs once.
void setup() {

    // Inititialze.
    initSerialOutput();
    initSensor();
}

// Main Code. Runs repeatedly.
void loop() {
    readSensor();
}

void initSerialOutput() {
    Serial.begin(BAUD_RATE);
}

void initSensor() {
    // Configure trigger and echo.
    pinMode(TRIGGER, OUTPUT);
    pinMode(ECHO, INPUT);
}

void readSensor() {
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(TRIGGER, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER, LOW);

    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    //pinMode(ECHO, INPUT);
    duration = pulseIn(ECHO, HIGH);

    // Convert the time into a distance
    cm = (duration / 2) / 29.1; // Divide by 29.1 or multiply by 0.0343
    inches = (duration / 2) / 74; // Divide by 74 or multiply by 0.0135
    
    // Output info.
    sprintf(str, "%.3f in. %.3f cm\n", inches, cm);
    Serial.print(str);

    // Delay to pace reading.
    delay(DELAY_LEN);
}





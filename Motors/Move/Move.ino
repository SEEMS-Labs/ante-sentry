// Pin Mapping
/*
ESP32 ----- DRV8833
GPIO 4 <---> IN 1
GPIO 5 <---> IN 2
GPIO 6 <---> IN 3
GPIO 7 <---> IN 4
*/

#include "driver/mcpwm.h"

// LED PWM Stuff;
#define FREQ 20000
#define RES 8
#define LED_C_LOW 0
#define LED_C_HIGH 255

// Define pins.
#define R_MOT_1 4
#define R_MOT_2 5
#define L_MOT_1 6
#define L_MOT_2 7

// Serial monitor.
#define BAUD_RATE 9600

// Define cases.
#define BOTH_MOTORS_ON  0 
#define BOTH_MOTORS_OFF 1
#define LEFT_MOTOR_ON   2
#define RIGHT_MOTOR_ON  3
#define SPEED_UP_LEFT   4
#define SPEED_UP_RIGHT  5
#define SPEED_TEST 6

void setup() {


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

  // Serial Monitor.
  Serial.begin(BAUD_RATE);  

}

void loop() {
  // Take keyboard input and decide what to do.
  if (Serial.available() > 0) {
    int prompt = Serial.parseInt();
    if (prompt >= 0) {
      Serial.printf("Performing command: %d\n", prompt);
      performCommand(prompt);
    }
    else {
      Serial.printf("\nDone.: %d\n", prompt);
      //vTaskDelete(NULL);
    }
  }
}


// Function to turn a Motor off.
void motorStop(int in1, int in2) {
  ledcWrite(in1, LED_C_LOW);
  ledcWrite(in2, LED_C_LOW);
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

// Function to make a motor turn clockwise with varying speed.
void motorVariableCW(int in1, int in2, int speed) {
  ledcWrite(in1, LED_C_LOW);
  ledcWrite(in2, speed);
}

// Function to make a motor turn counter clockwise with varying speed.
void motorVariableCCW(int in1, int in2, int speed) {
  ledcWrite(in1, speed);
  ledcWrite(in2, LED_C_LOW);
}

void performCommand(int command) {
  
  int speed = 0;
  switch(command) {
    case BOTH_MOTORS_ON:
      motorCW(L_MOT_1, L_MOT_2);
      motorCW(R_MOT_1, R_MOT_2);
      break;
    case BOTH_MOTORS_OFF:
      motorStop(L_MOT_1, L_MOT_2);
      motorStop(R_MOT_1, R_MOT_2);
      break;
    case LEFT_MOTOR_ON:
      motorCW(L_MOT_1, L_MOT_2);
      delay(3000);  // 3 Seconds.
      motorStop(L_MOT_1, L_MOT_2);
      break;
    case RIGHT_MOTOR_ON:
      motorCW(R_MOT_1, R_MOT_2);
      delay(3000);  // 3 Seconds.
      motorStop(R_MOT_1, R_MOT_2);
      break;
    case SPEED_UP_LEFT: 
      for(int speed = 0; speed < 256; speed++) {
        motorVariableCW(L_MOT_1, L_MOT_2, speed);
        Serial.printf("Speed: %d\n", speed);
        delay(200);
      }
      motorStop(L_MOT_1, L_MOT_2);
      break;
    case SPEED_UP_RIGHT:
      for(int speed = 0; speed < 256; speed++) {
        motorVariableCW(R_MOT_1, R_MOT_2, speed);
        Serial.printf("Speed: %d\n", speed);
        delay(200);
      }
      motorStop(R_MOT_1, R_MOT_2);
      break;
    
    case SPEED_TEST:
      while(Serial.available() <= 0) {}
      speed = Serial.parseInt();
      motorVariableCW(R_MOT_1, R_MOT_2, speed);
      delay(3000);
      motorStop(R_MOT_1, R_MOT_2);
      break;

    // Turn both motors off in the default case.
    default:
      Serial.printf("Errant Command: %d, not supported\n", command);
      motorStop(L_MOT_1, L_MOT_2);
      motorStop(R_MOT_1, R_MOT_2);
      break;
  }
}
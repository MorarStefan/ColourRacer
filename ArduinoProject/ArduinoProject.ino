#include <Servo.h>
#include "WiFi.h"
#include "ColorSensor.h"

// Pins of motor 1
#define mpin00 5
#define mpin01 6
// Pins of motor 2
#define mpin10 3
#define mpin11 11

// Pin of servomotor
#define smpin0 8

Servo srv;
boolean start = false, speedUp = false, speedDown = false, controllerChanged = false;
int currentSpeed = 0, defaultSpeed = 60, slowSpeed = 10, fastSpeed = 120;

void setupMotors() {
  // configuration of motor pins as output, initially 0
  digitalWrite(mpin00, 0);
  digitalWrite(mpin01, 0);
  digitalWrite(mpin10, 0);
  digitalWrite(mpin11, 0);
  pinMode(mpin00, OUTPUT);
  pinMode(mpin01, OUTPUT);
  pinMode(mpin10, OUTPUT);
  pinMode(mpin11, OUTPUT);
  // LED pin
  pinMode(13, OUTPUT);
}

void setup() {
  Serial.begin(REMOTEXY_SERIAL_SPEED);
  initializeColorSensor();
  RemoteXY_Init ();
  //setupMotors();
  //warningBlinking();
  //currentSpeed = defaultSpeed;
}


// Function to control a motor
// Input: pins m1 and m2, direction and speed
void StartMotor(int m1, int m2, int forward, int speed) {
  if (speed == 0) { // stop
    digitalWrite(m1, 0);
    digitalWrite(m2, 0);
  }
  else {
    if (forward) {
      digitalWrite(m2, 0);
      analogWrite(m1, speed); // use PWM
    }
    else {
      digitalWrite(m1, 0);
      analogWrite(m2, speed);
    }
  }
}

// Safety function
// Commands motors to stop, then delays
void delayStopped(int ms) {
  StartMotor(mpin00, mpin01, 0, 0);
  StartMotor(mpin10, mpin11, 0, 0);
  delay(ms);
}

// Use of servo
// Set three angles
// When finished, the servo remains in the middle (90 degrees)
void playWithServo() {
  srv.attach(smpin0);
  srv.write(0);
  delay(1000);
  srv.write(180);
  delay(1000);
  srv.write(90);
  delay(1000);
  srv.detach();
}

// Warning code
// Slow blink
void warningBlinking() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(13, 1);
    delay(200);
    digitalWrite(13, 0);
    delay(200);
  }
  // Fast blink. Remove the USB cable!!!!
  for (int i = 0; i < 10; i++) {
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
  }
  digitalWrite(13, 1);
}

void moveToTheFront(int speed, int duration) {
  StartMotor(mpin00, mpin01, 1, speed);
  StartMotor(mpin10, mpin11, 1, speed);
  delay(duration);
}

void moveToTheBack(int speed, int duration) {
  StartMotor(mpin00, mpin01, 0, speed);
  StartMotor(mpin10, mpin11, 0, speed);
  delay(duration);
}

void turnRight(int speed, int duration) {
  StartMotor(mpin00, mpin01, 1, speed);
  StartMotor(mpin10, mpin11, 1, speed / 3);
  delay(duration);
}

void turnLeft(int speed, int duration) {
  StartMotor(mpin00, mpin01, 1, speed / 3);
  StartMotor(mpin10, mpin11, 1, speed);
  delay(duration);
}

// The function returns: 0 (white), 1 (black), 2 (red), 3 (green), 4 (blue), 5 (yellow)
int detectColor() {

  if (R == 255 && G == 255 && B == 255) { // white


    strcpy(RemoteXY.colorText, "White");
    return 0;
  }
  else if (R < 30 && G < 30 && B < 30) { // black


    strcpy(RemoteXY.colorText, "Black");
    return 1;
  }
  else if (R > 150 && G < 100 && B < 100) { // red


    strcpy(RemoteXY.colorText, "Red");
    return 2;
  }
  else if (R < 50 && G > 50 && B > 50) { // green


    strcpy(RemoteXY.colorText, "Green");
    return 3;
  }
  else if (R > 100 && G > 100 && B > 150) { // blue


    strcpy(RemoteXY.colorText, "Blue");
    return 4;
  }
  else if (R > 150 && G > B) { // yellow

    strcpy(RemoteXY.colorText, "Yellow");
    return 5;
  }
  else {
    strcpy(RemoteXY.colorText, "None");
    return -1; // floor
  }
}

void changeRobotState() {
  int color = detectColor();
  if (color == 0) {
    if (start == false) { // (only if the state changes)
      // send message to the phone: WHITE
    }
    start = true, speedUp = false, speedDown = false, controllerChanged = false; // reset everything
  }
  else if (color == 1 || color == 2) {
    if (start == true) { // (only if the state changes)
      if (color == 1) {
        // send message to the phone: BLACK
      }
      else {
        // send message to the phone: RED
      }
    }
    start = false;
  }
  else if (color == 3) {
    if (speedUp == false) { // (only if the state changes)
      // send message to the phone: GREEN
    }
    speedUp = true; speedDown = false; controllerChanged = false;
  }
  else if (color == 4) {
    if (speedDown == false) { // (only if the state changes)
      // send message to the phone: BLUE
    }
    speedUp = false; speedDown = true; controllerChanged = false;
  }
  else if (color == 5) {
    if (controllerChanged == false) { // (only if the state changes)
      // send message to the phone: YELLOW
    }
    speedUp = false; speedDown = false; controllerChanged = true;
  }
}

void loop() {
  RemoteXY_Handler();
  readSensor();
  detectColor();

  //changeRobotState();
  /*
    start = false;
    if (start == true) {

    // Speed
    if (speedUp == true) {
      currentSpeed = fastSpeed;
    }
    else if (speedDown == true) {
      currentSpeed = slowSpeed;
    }
    else {
      currentSpeed = defaultSpeed;
    }

    // Controller
    if (RemoteXY.button_up == 1) {
      moveToTheFront(currentSpeed, 0);
    }
    else if (RemoteXY.button_down == 1) {
      moveToTheBack(currentSpeed, 0);
    }
    else if (RemoteXY.button_left == 1) {
      if (controllerChanged == false) {
        turnLeft(currentSpeed, 0);
      }
      else {
        turnRight(currentSpeed, 0);
      }
    }
    else if (RemoteXY.button_right == 1) {
      if (controllerChanged == false) {
        turnRight(currentSpeed, 0);
      }
      else {
        turnLeft(currentSpeed, 0);
      }
    }
    else {
      delayStopped(1);
    }
    }
    else {
    delayStopped(1);
    }
  */

}

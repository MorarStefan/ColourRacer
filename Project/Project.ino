#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT

#include <RemoteXY.h>
#include <Servo.h>

// RemoteXY connection settings
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "Krypton36"
#define REMOTEXY_SERVER_PORT 6377

// Pins of motor 1
#define mpin00 5
#define mpin01 6
// Pins of motor 2
#define mpin10 3
#define mpin11 11

// Pin of servomotor
#define smpin0 8

// Pins of color sensor
#define cspin0 13
#define cspin1 12
#define cspin2 10
#define cspin3 9
#define cspinOut 7

Servo srv;
int redFrequency = 0, greenFrequency = 0, blueFrequency = 0;
boolean start = false, speedUp = false, speedDown = false, controllerChanged = false;
int currentSpeed = 0, defaultSpeed = 60, slowSpeed = 10, fastSpeed = 120;

// RemoteXY configurate
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
{ 255, 4, 0, 0, 0, 43, 0, 8, 8, 1,
  1, 0, 27, 53, 12, 12, 13, 31, 94, 0,
  1, 0, 13, 67, 12, 12, 13, 31, 60, 0,
  1, 0, 41, 67, 12, 12, 13, 31, 62, 0,
  1, 0, 27, 80, 12, 12, 13, 31, 118, 0
};

// this structure defines all the variables of your control interface
struct {
  // input variable
  uint8_t button_up; // =1 if button pressed, else =0
  uint8_t button_left; // =1 if button pressed, else =0
  uint8_t button_right; // =1 if button pressed, else =0
  uint8_t button_down; // =1 if button pressed, else =0

  // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)

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

void setupColorSensor() {
  pinMode(cspin0, OUTPUT);
  pinMode(cspin1, OUTPUT);
  pinMode(cspin2, OUTPUT);
  pinMode(cspin3, OUTPUT);
  pinMode(cspinOut, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(cspin0, 1);
  digitalWrite(cspin1, 0);
}

void setup() {
  RemoteXY_Init ();
  setupMotors();
  setupColorSensor();
  warningBlinking();
  currentSpeed = defaultSpeed;
}

void readRGBfrequency() {
  // Red
  digitalWrite(cspin2, 0);
  digitalWrite(cspin3, 0);
  redFrequency = pulseIn(cspinOut, 0);
  // Remaping the value to the RGB Model
  redFrequency = map(redFrequency, 25, 70, 255, 0);
  delay(100);

  // Green
  digitalWrite(cspin2, 1);
  digitalWrite(cspin3, 1);
  greenFrequency = pulseIn(cspinOut, 0);
  // Remaping the value to the RGB Model
  greenFrequency = map(greenFrequency, 30, 90, 255, 0);
  delay(100);

  // Blue
  digitalWrite(cspin2, 0);
  digitalWrite(cspin3, 1);
  blueFrequency = pulseIn(cspinOut, 0);
  // Remaping the value to the RGB Model
  blueFrequency = map(blueFrequency, 25, 70, 255, 0);
  delay(100);
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
  readRGBfrequency();

  if (redFrequency > 200 && greenFrequency > 200 && blueFrequency > 200) { // white
    return 0;
  }
  else if (redFrequency < 30 && greenFrequency < 30 && blueFrequency < 30) { // black
    return 1;
  }
  else if (redFrequency > 100 && greenFrequency < 50 && blueFrequency < 50) { // red
    return 2;
  }
  else if (greenFrequency > redFrequency && greenFrequency > blueFrequency) { // green
    return 3;
  }
  else if (blueFrequency > redFrequency && blueFrequency > greenFrequency) { // blue
    return 4;
  }
  else if (redFrequency > 150 && greenFrequency > 150 && blueFrequency < 100) { // yellow
    return 5;
  }
  else {
    return -1; // floor
  }
}

void changeRobotState() {
  int color = detectColor();
  if (color == 0) {
     if(start == false) { // (only if the state changes)
      // send message to the phone: WHITE
    }
    start = true, speedUp = false, speedDown = false, controllerChanged = false; // reset everything
  }
  else if (color == 1 || color == 2) {
     if(start == true) { // (only if the state changes)
      if(color == 1) {
        // send message to the phone: BLACK
      }
      else {
        // send message to the phone: RED
      }
    }
    start = false;
  }
  else if (color == 3) {
    if(speedUp == false) { // (only if the state changes)
      // send message to the phone: GREEN
    }
    speedUp = true; speedDown = false; controllerChanged = false;
  }
  else if (color == 4) {
     if(speedDown == false) { // (only if the state changes)
      // send message to the phone: BLUE
    }
    speedUp = false; speedDown = true; controllerChanged = false;
  }
  else if (color == 5) {
     if(controllerChanged == false) { // (only if the state changes)
      // send message to the phone: YELLOW
    }
    speedUp = false; speedDown = false; controllerChanged = true;
  }
}

void loop() {
  RemoteXY_Handler();
  changeRobotState();

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
}

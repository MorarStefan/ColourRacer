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

  Serial.begin(9600);
}

void readRGBfrequency() {
  digitalWrite(cspin2, 0);
  digitalWrite(cspin3, 0);
  redFrequency = pulseIn(cspinOut, 0);
  Serial.print("R= ");
  Serial.print(redFrequency);
  Serial.print("  ");
  delay(100);

  digitalWrite(cspin2, 1);
  digitalWrite(cspin3, 1);
  greenFrequency = pulseIn(cspinOut, 0);
  Serial.print("G= ");
  Serial.print(greenFrequency);
  Serial.print("  ");
  delay(100);

  digitalWrite(cspin2, 0);
  digitalWrite(cspin3, 1);
  blueFrequency = pulseIn(cspinOut, 0);
  Serial.print("B= ");
  Serial.print(blueFrequency);
  Serial.println("  ");
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
}

void turnLeft(int speed, int duration) {
  StartMotor(mpin00, mpin01, 1, speed / 3);
  StartMotor(mpin10, mpin11, 1, speed);
  delay(duration);
}

void loop() {
  readRGBfrequency();

  RemoteXY_Handler();

  if (RemoteXY.button_up == 1) {
    moveToTheFront(120, 0);
  }
  else if (RemoteXY.button_down == 1) {
    moveToTheBack(120, 0);
  }
  else if (RemoteXY.button_left == 1) {
    turnLeft(120, 0);
  }
  else if (RemoteXY.button_right == 1) {
    turnRight(120, 0);
  }
  else {
    delayStopped(1);
  }
}

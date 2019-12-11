// Check: https://howtomechatronics.com/tutorials/arduino/arduino-color-sensing-tutorial-tcs230-tcs3200-color-sensor/

// Pins of color sensor
#define cspin0 13
#define cspin1 12
#define cspin2 10
#define cspin3 9
#define cspinOut 7

int redFrequency = 0, greenFrequency = 0, blueFrequency = 0;

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
  setupColorSensor();
  Serial.begin(9600);
}

void readRGBfrequency() {
  // Red 
  digitalWrite(cspin2, 0);
  digitalWrite(cspin3, 0);
  redFrequency = pulseIn(cspinOut, 0);
  // Remaping the value to the RGB Model
  redFrequency = map(redFrequency, 25, 70, 255, 0);
  Serial.print("R: ");
  Serial.print(redFrequency);
  Serial.print("  ");
  delay(100);

  // Green
  digitalWrite(cspin2, 1);
  digitalWrite(cspin3, 1);
  greenFrequency = pulseIn(cspinOut, 0);
  // Remaping the value to the RGB Model
  greenFrequency = map(greenFrequency, 30, 90, 255, 0);
  Serial.print("G: ");
  Serial.print(greenFrequency);
  Serial.print("  ");
  delay(100);

  // Blue
  digitalWrite(cspin2, 0);
  digitalWrite(cspin3, 1);
  blueFrequency = pulseIn(cspinOut, 0);
  // Remaping the value to the RGB Model
  blueFrequency = map(blueFrequency, 25, 70, 255, 0);
  Serial.print("B: ");
  Serial.print(blueFrequency);
  Serial.println("  ");
  delay(100);
}

void loop() {
  readRGBfrequency();
}

//Color Sensor libraries
#include <MD_TCS230.h>
#include <FreqCount.h>

// ColorSensor Pin definitions
#define  S2_OUT  2
#define  S3_OUT  3
#define  OE_OUT   4    // LOW = ENABLED 
#define cs   10  
#define dc   9   
#define rst  8   
////////////////

////////////////////////////////////////For detecting color - SensorData
#define  DEBUG_TCS230 0   ///< Debug flag. Set to 1 to enable debug output to Serial.

#if  DEBUG_TCS230
#define DUMP(s, v)  { Serial.print(F(s)); Serial.print(v); }  ///< Debug label + value
#define DUMPS(s)    { Serial.print(F(s)); }                   ///< Debug label only
#else
#define DUMP(s, v)  ///< Debug label + value
#define DUMPS(s)    ///< Debug label only
#endif
//////////////////////////////////////

/////////////////// Set the Sensor Data by hand
int state = 0;
sensorData sdBlack = { 11770, 11360, 15190 };
sensorData sdWhite = { 21984, 24104, 54574 };
///////////////////

//////////////Other color sensor data
int R = 0;
int G = 0;
int B = 0;

MD_TCS230 CS(S2_OUT, S3_OUT, OE_OUT);
//////////////////////

void readInitialColorSensorData() {
  if (state == 0)
  {
    if (CS.available()) {
      sensorData  sd;
      CS.getRaw(&sd);
      for (uint8_t i = 0; i < RGB_SIZE; i++)
      {
        DUMP(" ", sd->value[i]);
        Serial.println((unsigned int)sd.value[i]);
      }
      CS.setDarkCal(&sdBlack);
      Serial.println("Black Calibration Set");
      state++;
      delay(5000);
      CS.read();
    }
  }
  else if (state == 1)
  {
    if (CS.available()) {
      sensorData  sd;
      CS.getRaw(&sd);
      for (uint8_t i = 0; i < RGB_SIZE; i++)
      {
        DUMP(" ", sd->value[i]);
        Serial.println((unsigned int)sd.value[i]);
      }
      CS.setWhiteCal(&sdWhite);
      Serial.println("White Calibration Set");
      state++;
    }
  }
}

void initializeColorSensor() {
  CS.begin();
  CS.read();
  readInitialColorSensorData();
}

void readSensor()
{
  static  bool  waiting = false;

  if (!waiting)
  {
    CS.read();
    waiting = true;
  }
  else
  {
    if (CS.available())
    {
      colorData  rgb;

      CS.getRGB(&rgb);
      Serial.print("RGB [");
      Serial.print(rgb.value[TCS230_RGB_R]);
      Serial.print(",");
      Serial.print(rgb.value[TCS230_RGB_G]);
      Serial.print(",");
      Serial.print(rgb.value[TCS230_RGB_B]);
      Serial.println("]");

      R = rgb.value[TCS230_RGB_R];
      G = rgb.value[TCS230_RGB_G];
      B = rgb.value[TCS230_RGB_B];

      delay(100);

      waiting = false;
    }
  }
}

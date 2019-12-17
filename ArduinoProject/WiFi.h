#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT
#include <RemoteXY.h>

// RemoteXY connection settings
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "Krypton36"
#define REMOTEXY_SERVER_PORT 6377

// RemoteXY configurate
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
{ 255, 4, 0, 11, 0, 52, 0, 8, 24, 1,
  1, 0, 27, 53, 12, 12, 13, 31, 94, 0,
  1, 0, 13, 67, 12, 12, 13, 31, 60, 0,
  1, 0, 41, 67, 12, 12, 13, 31, 62, 0,
  1, 0, 27, 80, 12, 12, 13, 31, 118, 0,
67,4,13,18,34,15,16,26,11 };

// this structure defines all the variables of your control interface
struct {
  // input variable
  uint8_t button_up; // =1 if button pressed, else =0
  uint8_t button_left; // =1 if button pressed, else =0
  uint8_t button_right; // =1 if button pressed, else =0
  uint8_t button_down; // =1 if button pressed, else =0
  // output variable
  char colorText[11];  // string UTF8 end zero 

  // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)

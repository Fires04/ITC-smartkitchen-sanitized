#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define BowlingLed 2
#define StatekLed 4

#define StatusLed 14
#define StatusStatekLed 13
#define StatusBowlingLed 12

#define BuzzerPin 15

#define HOST_NAME "SmartKitchen-Kitchen"
#define WIFI_SSID "your-wifi-ssid"  // your network SSID (name)
#define WIFI_PASS "your-wifi-password"  // your network key

#define COMMAND_PSK "your-psk"
#define COMMAND_TURN_ON "turnOn"

//#define STATEK1_IP "10.0.10.250"
//#define STATEK2_IP "10.0.10.249"
//#define BOWLING_IP "10.0.10.248"
//#define HTTP_PORT 80

#define INTERUPT_TIMEOUT 1000

#if defined ESP8266 || defined ESP32 //check board
  #define USE_MDNS true
  #define USE_ARDUINO_OTA true
  #define WEB_SERVER_ENABLED true
  //#define DEBUG_DISABLED true
#else
  #error "The board must be ESP8266 or ESP32"
#endif // ESP

#endif
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define StatusLed 12
#define KitchenLed 14
#define ButtonLed 2

#define BuzzerPin 15

#define HOST_NAME "SmartKitchen-Kitchen"
#define WIFI_SSID "your-wifi-ssid"  // your network SSID (name)
#define WIFI_PASS "your-wifi-password"  // your network key

#define COMMAND_PSK "your-psk"

#define KITCHEN_IP "your-ip"
#define ROOM_ID "bowling"

#define INTERUPT_TIMEOUT 3000

#if defined ESP8266 || defined ESP32 //check board
  #define USE_MDNS true
  #define USE_ARDUINO_OTA true
  #define WEB_SERVER_ENABLED true
  //#define DEBUG_DISABLED true
#else
  #error "The board must be ESP8266 or ESP32"
#endif // ESP

#endif
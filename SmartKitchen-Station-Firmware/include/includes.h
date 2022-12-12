/**
 * Includes file include all required libraries
 */ 
#ifndef INCLUDES_H
#define INCLUDES_H

#include <ESP8266WiFi.h>
#ifdef USE_MDNS
  #include <DNSServer.h>
  #include <ESP8266mDNS.h>
#endif
  #ifdef WEB_SERVER_ENABLED
    #include <ESP8266WebServer.h>
#endif

// Arduino OTA
#ifdef USE_ARDUINO_OTA
  #include <ArduinoOTA.h>
#endif

//Other Libraries
#include <jled.h>
#include <EasyBuzzer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <jm_Scheduler.h>


#endif //INCLUDES_H 
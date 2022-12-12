#include "../include/headerFiles.h"


bool fireStatekInterupt = false;
bool fireBowlingInterupt = false;
unsigned long interuptTime = 0;
unsigned long lastWifiTime = 0;

bool statekStatus = false;
unsigned long statekLastRequest = 0;


bool bowlingStatus = false;
unsigned long bowlingLastRequest = 0;



void connectWiFi(){
    // Connect WiFi
    Serial.println("*** connectWiFi: begin conection ...");
    // Connect with SSID and password stored
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    // Wait connection
    uint32_t timeout = millis() + 20000; // Time out
    while (WiFi.status() != WL_CONNECTED && millis() < timeout)
    {
        delay(250);
        Serial.print(".");
        if(millis() > (timeout - 2000)){
            Serial.println("Unable to connect to wifi - restart ");
            ESP.restart();
        }
    }
    // End
    Serial.println("");
    Serial.print("connectWiFi: connect a ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP().toString());
    ledStatus.On();
}

//callback functions
void initializeOTA() {
    #if defined ESP8266
        ArduinoOTA.onStart([]() {
            Serial.println("* OTA: Start");
        });
        ArduinoOTA.onEnd([]() {
            Serial.println("\n*OTA: End");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("*OTA: Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("*OTA: Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });
    #elif defined ESP32
    // ArduinoOTA
        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";
            Serial.println("Start updating " + type);
        }).onEnd([]() {
            Serial.println("\nEnd");
        }).onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        }).onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });
    #endif
    // Begin
  ArduinoOTA.begin();
}

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}

void ledStatekFastBlink(){
    if(!ledStatek.IsRunning()){
        ledStatek = JLed(StatekLed).Blink(250, 250).Forever();
    }
}

void ledStatekTurnOff(){
    ledStatek = JLed(StatekLed).Off();
}

void ledBowlingFastBlink(){
    if(!ledBowling.IsRunning()){
        ledBowling = JLed(BowlingLed).Blink(250, 250).Forever();
    }
}

void ledBowlingTurnOff(){
    ledBowling = JLed(BowlingLed).Off();
}

//webserver handles
void handleRoot()
{
    if(HTTPServer.arg("PSK") == COMMAND_PSK){
        if(HTTPServer.arg("cmd")== "getRequiredStatus" || HTTPServer.arg("cmd")== "turnOffSignal"){
            if(HTTPServer.arg("id")=="statek"){ 
                if(HTTPServer.arg("status")=="on"){
                    ledStatekFastBlink();
                }else{
                    ledStatekTurnOff();
                }
                statekLastRequest = millis();
                if(HTTPServer.arg("cmd")== "getRequiredStatus"){                    
                    HTTPServer.send(200, "text/plain", BoolToString(statekStatus));
                }else if(HTTPServer.arg("cmd")== "turnOffSignal"){
                    statekStatus = false;
                    HTTPServer.send(200, "text/plain", "ok");
                }
            }else if(HTTPServer.arg("id")=="bowling"){
                if(HTTPServer.arg("status")=="on"){
                    ledBowlingFastBlink();
                }else{
                    ledBowlingTurnOff();
                }
                bowlingLastRequest = millis();
                if(HTTPServer.arg("cmd")== "getRequiredStatus"){                    
                    HTTPServer.send(200, "text/plain", BoolToString(bowlingStatus));
                }else if(HTTPServer.arg("cmd")== "turnOffSignal"){
                    bowlingStatus = false;
                    HTTPServer.send(200, "text/plain", "ok");
                }
            }else{
                HTTPServer.send(500, "text/plain", "Uknown room");
            }
        }
        
    }else{
        HTTPServer.send(500, "text/plain", "PSKNotMatch");
    }   
}

void handleNotFound()
{
    HTTPServer.send(404, "text/plain", "");
}

void smartKitchen()
{
    //update leds and buzzer
    ledBowling.Update();
    ledStatek.Update();
    ledStatus.Update();
    ledStatusStatek.Update();
    ledStatusBowling.Update();
    EasyBuzzer.update();
    interuptFunctions();
    watchdogsCheck();
}

String getCommandUrl(String IP, String command)
{
    String url = "http://";
    url += IP;
    url += "/?PSK=";
    url += COMMAND_PSK;
    url += "&cmd=";
    url += command;
    return url;
}

void watchdogsCheck(){
    if(WiFi.status() == WL_CONNECTED){
        lastWifiTime = millis();
    }
    if((millis() - lastWifiTime) > 5000){
        Serial.println("Wifi lost - restart");
        ledStatus.Off();
        ledStatus.Update();
        delay(100);
        ESP.reset();
    }
    if((millis() - statekLastRequest) > 5000){
         ledStatusStatek = JLed(StatusStatekLed).Off();
    }else{
         ledStatusStatek = JLed(StatusStatekLed).On();
    }
    if((millis() - bowlingLastRequest) > 5000){
         ledStatusBowling = JLed(StatusBowlingLed).Off();
    }else{
         ledStatusBowling = JLed(StatusBowlingLed).On();
    }
    if(statekStatus){
        if(!ledStatek.IsRunning()){
            ledStatek = JLed(StatekLed).Blink(250, 250).Forever();
        }
    }else{
        ledStatek = JLed(StatekLed).Off();
    }
    if(bowlingStatus){
        if(!ledBowling.IsRunning()){
            ledBowling = JLed(BowlingLed).Blink(250, 250).Forever();
        }
    }else{
        ledBowling = JLed(BowlingLed).Off();
    }
}

bool checkInterupt(){
    if ((millis() - interuptTime) < INTERUPT_TIMEOUT)
    {
        Serial.print("Under time interupt\n");
        return false;
    }
    else
    {
        Serial.print("Success interupt\n");
        interuptTime = millis();
        return true;
    }
}

//callbacks
void bowlingCall()
{
    Serial.println("Bowling call");
    if(checkInterupt()){
        fireBowlingInterupt = true;
    }
}

void statekCall()
{
    Serial.println("Statek call");
    if(checkInterupt()){
        fireStatekInterupt = true;
    }
}


void interuptFunctions(){
    if(fireBowlingInterupt){
        bowlingStatus = true;
        fireBowlingInterupt = false;
    }
    if(fireStatekInterupt){
        statekStatus = true;
        fireStatekInterupt = false;
    }

}

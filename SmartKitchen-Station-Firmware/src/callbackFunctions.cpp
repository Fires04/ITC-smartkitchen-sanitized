#include "../include/headerFiles.h"


bool fireButtonCall = false;
//bool turnOffMaster = false;
bool currentStatus = false;

unsigned long interuptTime = 0;
unsigned long lastWifiTime = 0;

unsigned long kitchenLastConnection = 0;

WiFiClient client;

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

inline const char * const BoolToString(bool b){
  return b ? "true" : "false";
}

//webserver handles
void handleRoot(){
    if(HTTPServer.arg("PSK") == COMMAND_PSK){
                HTTPServer.send(200, "text/plain", "Client online!");
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
    ledKitchen.Update();
    ledStatus.Update();
    ledButton.Update();
    EasyBuzzer.update();
    interuptFunctions();
    watchdogsCheck();
}

String getCommandUrl(String IP, String command,String id,String status){
    String url = "http://";
    url += IP;
    url += "/?PSK=";
    url += COMMAND_PSK;
    url += "&cmd=";
    url += command;
    url += "&id=";
    url += id;
    url += "&status=";
    url += status;
    return url;
}

void watchdogsCheck(){
    if(WiFi.status() == WL_CONNECTED){
        lastWifiTime = millis();
    }
    if((millis() - lastWifiTime) > 3000){
        Serial.println("Wifi lost - restart");
        ledStatus.Off();
        ledStatus.Update();
        delay(100);
        ESP.reset();
    }
    if((millis() - kitchenLastConnection) > 5000){
         //Serial.println("Kitchen connection lost");
         ledKitchen = JLed(KitchenLed).Off();
    }else{
         ledKitchen = JLed(KitchenLed).On();
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

void buzzerAlarm(){
    EasyBuzzer.setPin(BuzzerPin);
    EasyBuzzer.beep(
       2000, // Frequency in hertz(HZ).
        250,  // On Duration in milliseconds(ms).
        250,  // Off Duration in milliseconds(ms).
        999,    // The number of beeps per cycle.
        500,  // Pause duration.
        1     // The number of cycle.
    );
}

void buzzerError(){
    EasyBuzzer.setPin(BuzzerPin);
    EasyBuzzer.beep(
        3000, // Frequency in hertz(HZ).
        200,  // On Duration in milliseconds(ms).
        200,  // Off Duration in milliseconds(ms).
        2,    // The number of beeps per cycle.
        500,  // Pause duration.
        1     // The number of cycle.
    );
}

//callbacks
void buttonCall()
{
    Serial.println("Button call");
    if(checkInterupt()){
        fireButtonCall = true;
    }
}

void interuptFunctions(){
    if(fireButtonCall){
        fireButtonCall = false;
        sendTurnOffCommand();
        turnOffAlarm();
    }
}

void getStatusFromMaster(){
    HTTPClient http;
    http.setReuse(false);
    http.setTimeout(1500);
    http.begin(client, getCommandUrl(KITCHEN_IP,"getRequiredStatus", ROOM_ID, BoolToString(currentStatus)));
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        kitchenLastConnection = millis();
        //Serial.println(payload);
        if(payload == "true"){
            Serial.println("turnOnAlarm");
            turnOnAlarm();
        }else{
            Serial.println("turnOffAlarm");
            turnOffAlarm();
        }
      }
    } else {
        Serial.printf("[HTTP] GET - getStatusFromMaster - ... failed, error: %s\n", http.errorToString(httpCode).c_str());
        ledKitchen = JLed(KitchenLed).Off();
    }
    http.end();
}


void sendTurnOffCommand(){
    HTTPClient http;
    http.setReuse(false);
    http.setTimeout(1500);
    http.begin(client, getCommandUrl(KITCHEN_IP,"turnOffSignal",ROOM_ID,BoolToString(currentStatus)));
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        kitchenLastConnection = millis();
        Serial.println(payload);
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        ledKitchen = JLed(KitchenLed).Off();
        buzzerError();
    }
    http.end();
}


void turnOnAlarm(){
    if(!ledButton.IsRunning()){
        Serial.println("ButtonLed turn on");
        ledButton = JLed(ButtonLed).Blink(250, 250).Forever();
        buzzerAlarm();
    }
}

void turnOffAlarm(){
    ledButton = JLed(ButtonLed).Off();
    EasyBuzzer.setPin(BuzzerPin);
    EasyBuzzer.beep(
        2000, // Frequency in hertz(HZ).
        500,  // On Duration in milliseconds(ms).
        500,  // Off Duration in milliseconds(ms).
        0,    // The number of beeps per cycle.
        500,  // Pause duration.
        0     // The number of cycle.
    );
    EasyBuzzer.stopBeep();
}
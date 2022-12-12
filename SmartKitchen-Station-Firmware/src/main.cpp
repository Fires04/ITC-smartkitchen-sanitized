/**
 * SmartKitchen - Kitchen firmware
 * @version v0.0
 * @board Lolin V3 - Nodemcu clone - ESP8266
 * @author David Stein <info@davidstein.cz> 
 */
#include "../include/headerFiles.h"
#include "RTCVars.h"

uint32_t mTimeToSec = 0;
uint32_t mTimeSeconds = 0;



void setup()
{
    //pinmode setup

    pinMode(KitchenLed, OUTPUT);
    pinMode(StatusLed, OUTPUT);
    pinMode(ButtonLed, OUTPUT);
    digitalWrite(StatusLed, LOW);
    digitalWrite(KitchenLed, LOW);
    digitalWrite(ButtonLed, HIGH);

    pinMode(digitalPinToInterrupt(D3), INPUT);

    attachInterrupt(digitalPinToInterrupt(D3), buttonCall, FALLING);

    Serial.begin(230400);
     // let's inform the user about what type of reset has happened
    rst_info *resetInfo;
    resetInfo = ESP.getResetInfoPtr();

    Serial.print("Reset Reason was: ");
    Serial.println(resetInfo->reason);


    connectWiFi();
    WiFi.hostname(HOST_NAME);
#ifdef USE_ARDUINO_OTA
    // Update over air (OTA)
    initializeOTA();
#endif

//
// Register the services
//
//mDNS
#if defined USE_MDNS && defined HOST_NAME
    if (MDNS.begin(HOST_NAME))
    {
        Serial.print("* MDNS responder started. Hostname -> ");
        Serial.println(HOST_NAME);
    }
// Register the services
#ifdef WEB_SERVER_ENABLED
    Serial.println("Web service start");
    MDNS.addService("http", "tcp", 80); // Web server
#endif
#endif // MDNS

// HTTP web server
#ifdef WEB_SERVER_ENABLED
    HTTPServer.on("/", handleRoot);
    HTTPServer.onNotFound(handleNotFound);
    HTTPServer.begin();
    Serial.println("* HTTP server started");
#endif



    EasyBuzzer.setPin(BuzzerPin);

    EasyBuzzer.beep(
        2000, // Frequency in hertz(HZ).
        500,  // On Duration in milliseconds(ms).
        500,  // Off Duration in milliseconds(ms).
        2,    // The number of beeps per cycle.
        500,  // Pause duration.
        1     // The number of cycle.
    );

    scheduler.start(getStatusFromMaster, TIMESTAMP_1SEC*3); // Start immediately coroutine() and repeat it every second
}

void loop()
{

#ifdef USE_ARDUINO_OTA
    // Update over air (OTA)
    ArduinoOTA.handle();
#endif

#ifdef WEB_SERVER_ENABLED
    // Web server
    HTTPServer.handleClient();
#endif

    // Give a time for ESP
    yield();

    //SmartKitchen processes
    smartKitchen();

}


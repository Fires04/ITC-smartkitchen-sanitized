/**
 * SmartKitchen - Kitchen firmware
 * @version v0.0
 * @board Lolin V3 - Nodemcu clone - ESP8266
 * @author David Stein <info@davidstein.cz> 
 */
#include "../include/headerFiles.h"

uint32_t mTimeToSec = 0;
uint32_t mTimeSeconds = 0;

void setup()
{
    //pinmode setup

    pinMode(BowlingLed, OUTPUT);
    pinMode(StatekLed, OUTPUT);
    pinMode(StatusLed, OUTPUT);
    digitalWrite(StatusLed, LOW);
    
    pinMode(StatusStatekLed, OUTPUT);
    pinMode(StatusBowlingLed, OUTPUT);

    pinMode(digitalPinToInterrupt(D3), INPUT);
    pinMode(digitalPinToInterrupt(D1), INPUT);

    attachInterrupt(digitalPinToInterrupt(D3), bowlingCall, FALLING);
    attachInterrupt(digitalPinToInterrupt(D1), statekCall, FALLING);



    Serial.begin(230400);
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
        3000, // Frequency in hertz(HZ).
        500,  // On Duration in milliseconds(ms).
        500,  // Off Duration in milliseconds(ms).
        2,    // The number of beeps per cycle.
        500,  // Pause duration.
        1     // The number of cycle.
    );

    interuptTime = 0;

    //scheduler.start(watchdogStatek, TIMESTAMP_1SEC*2); // Start immediately coroutine() and repeat it every second
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


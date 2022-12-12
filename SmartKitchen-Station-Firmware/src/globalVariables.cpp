#include "../include/headerFiles.h"

//led variables
JLed ledStatus = JLed(StatusLed).Blink(250, 250).Repeat(2);
JLed ledKitchen = JLed(KitchenLed).Blink(250, 250).Repeat(5);
JLed ledButton = JLed(ButtonLed).Blink(250, 250).Repeat(5);

unsigned long lastInteruptTime = 0;
bool fireHttpClient = false;

jm_Scheduler scheduler;

//web serrver
#ifdef WEB_SERVER_ENABLED
    ESP8266WebServer HTTPServer(80);
#endif // WEB_SERVER_ENABLED

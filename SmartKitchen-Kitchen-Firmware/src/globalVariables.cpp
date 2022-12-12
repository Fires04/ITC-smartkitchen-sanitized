#include "../include/headerFiles.h"

//led variables
JLed ledBowling = JLed(BowlingLed).Blink(250, 250).Repeat(5);
JLed ledStatek = JLed(StatekLed).Blink(250, 250).Repeat(5);
JLed ledStatus = JLed(StatusLed).Blink(250, 250).Repeat(2);
JLed ledStatusStatek = JLed(StatusStatekLed).Blink(250, 250).Repeat(5);
JLed ledStatusBowling = JLed(StatusBowlingLed).Blink(250, 250).Repeat(5);

unsigned long lastInteruptTime = 0;
bool fireHttpClient = false;

jm_Scheduler scheduler;

//web serrver
#ifdef WEB_SERVER_ENABLED
    ESP8266WebServer HTTPServer(80);
#endif // WEB_SERVER_ENABLED

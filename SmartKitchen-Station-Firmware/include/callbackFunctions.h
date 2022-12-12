#ifndef CALLBACKFUNCTIONS_H
#define CALLBACKFUNCTIONS_H

void connectWiFi();
void initializeOTA();
void handleRoot();
void handleNotFound();

void buttonCall();
void sendTurnOffCommand();
void getStatusFromMaster();
void smartKitchen();
void interuptFunctions();
void watchdogsCheck();

void turnOnAlarm();
void turnOffAlarm();

void buzzerAlarm();
void buzzerError();

#endif
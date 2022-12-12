#ifndef CALLBACKFUNCTIONS_H
#define CALLBACKFUNCTIONS_H

void connectWiFi();
void initializeOTA();
void handleRoot();
void handleNotFound();

void bowlingCall();
void statekCall();

void smartKitchen();

String getCommandUrl(String IP, String command);
//interupts
void bowlingCall();
void statekCall();
void interuptFunctions();

bool bowlingCommand();
bool statekCommand();

bool checkInterupt();
bool getHttpCommand(String url);

bool errorBuzz();

void watchdogsCheck();


#endif
#pragma once
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

extern AsyncWebServer server;

extern unsigned long lastCheck;
extern bool apEnabled;

void enableAP();
void disableAP();

void wifiManagerLoop();

#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <esp_wifi.h>
#include <LittleFS.h>

#include "wifi_globals.h"

void wifiManagerSetup();

void handleScan(AsyncWebServerRequest *request);
void handleConnect(AsyncWebServerRequest *request);
void handleStatus(AsyncWebServerRequest *request);
void handleClients(AsyncWebServerRequest *request);

void enableAP();
void disableAP();

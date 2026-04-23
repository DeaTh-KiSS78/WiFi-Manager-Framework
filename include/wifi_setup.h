#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <esp_wifi.h>
#include <LittleFS.h>

extern AsyncWebServer server;
extern Preferences prefs;

bool loadWiFi(String &ssid, String &pass);
void saveWiFi(const String &ssid, const String &pass);
void clearWiFiCredentials();

void enableAP();
void disableAP();

void handleScan(AsyncWebServerRequest *request);
void handleConnect(AsyncWebServerRequest *request);
void handleStatus(AsyncWebServerRequest *request);
void handleClients(AsyncWebServerRequest *request);

void wifiManagerSetup();

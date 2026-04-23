#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

AsyncWebServer server(80);
Preferences prefs;

#include "wifi_setup.h"
#include "wifi_loop.h"

void setup() {
    Serial.begin(115200);
    wifiManagerSetup();
}

void loop() {
    wifiManagerLoop();
}

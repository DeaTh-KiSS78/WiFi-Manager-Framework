#pragma once
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

extern AsyncWebServer server;

// variabile interne pentru watchdog
unsigned long lastCheck = 0;
bool apEnabled = true;

// declarăm funcțiile din wifi_setup.h
void enableAP();
void disableAP();

void wifiManagerLoop() {
    // AsyncWebServer NU folosește handleClient()
    // deci eliminăm complet server.handleClient();

    // watchdog WiFi la fiecare 30 secunde
    if (millis() - lastCheck > 30000) {
        lastCheck = millis();

        // dacă suntem conectați la WiFi → dezactivăm AP
        if (WiFi.status() == WL_CONNECTED) {

            if (apEnabled) {
                disableAP();
                apEnabled = false;
            }

            Serial.print("[WiFi] Conectat. IP: ");
            Serial.println(WiFi.localIP());
        }

        // dacă am pierdut conexiunea → reactivăm AP
        else {

            if (!apEnabled) {
                enableAP();
                apEnabled = true;
            }

            Serial.println("[WiFi] Neconectat");
        }
    }
}

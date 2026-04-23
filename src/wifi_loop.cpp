#include "wifi_loop.h"

unsigned long lastCheck = 0;
bool apEnabled = true;

void wifiManagerLoop() {

    if (millis() - lastCheck > 30000) {
        lastCheck = millis();

        if (WiFi.status() == WL_CONNECTED) {

            if (apEnabled) {
                disableAP();
                apEnabled = false;
            }

            Serial.print("[WiFi] Conectat. IP: ");
            Serial.println(WiFi.localIP());
        }

        else {

            if (!apEnabled) {
                enableAP();
                apEnabled = true;
            }

            Serial.println("[WiFi] Neconectat");
        }
    }
}

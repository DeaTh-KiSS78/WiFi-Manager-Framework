#include "wifi_setup.h"

// -------------------------------------------------
// Funcții interne: load/save/clear credențiale WiFi
// -------------------------------------------------

bool loadWiFi(String &ssid, String &pass) {
    prefs.begin("wifi", true);
    ssid = prefs.getString("ssid", "");
    pass = prefs.getString("pass", "");
    prefs.end();
    return ssid.length() > 0;
}

void saveWiFi(const String &ssid, const String &pass) {
    prefs.begin("wifi", false);
    prefs.putString("ssid", ssid);
    prefs.putString("pass", pass);
    prefs.end();
}

void clearWiFiCredentials() {
    prefs.begin("wifi", false);
    prefs.putString("ssid", "");
    prefs.putString("pass", "");
    prefs.end();
    Serial.println("[WiFi] Credentials cleared");
}

// -------------------------------------------------
// Funcții pentru controlul AP
// -------------------------------------------------

void enableAP() {
    Serial.println("[WiFi] Enabling AP...");

    WiFi.mode(WIFI_AP_STA);

    IPAddress local_IP(192, 168, 5, 1);
    IPAddress gateway(192, 168, 5, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP("ESP-WIFI", "12345678");

    Serial.print("[WiFi] AP IP: ");
    Serial.println(WiFi.softAPIP());
}

void disableAP() {
    Serial.println("[WiFi] Disabling AP...");
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
}

// -------------------------------------------------
// Handlere Web (Async)
// -------------------------------------------------

void handleScan(AsyncWebServerRequest *request) {
    int n = WiFi.scanComplete();

    if (n == WIFI_SCAN_RUNNING) {
        request->send(200, "application/json", "[]");
        return;
    }

    if (n == WIFI_SCAN_FAILED) {
        WiFi.scanNetworks(true);
        request->send(200, "application/json", "[]");
        return;
    }

    String json = "[";
    for (int i = 0; i < n; i++) {
        json += "\"" + WiFi.SSID(i) + "\"";
        if (i < n - 1) json += ",";
    }
    json += "]";

    WiFi.scanDelete();
    WiFi.scanNetworks(true);

    request->send(200, "application/json", json);
}

void handleConnect(AsyncWebServerRequest *request) {
    String ssid = request->arg("ssid");
    String pass = request->arg("pass");

    WiFi.begin(ssid.c_str(), pass.c_str());
    request->send(200, "text/plain", "connecting");

    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 8000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        saveWiFi(ssid, pass);
    }
}

void handleStatus(AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
    json += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";
    json += "}";
    request->send(200, "application/json", json);
}

void handleClients(AsyncWebServerRequest *request) {
    wifi_sta_list_t stationList;
    tcpip_adapter_sta_list_t adapterList;

    esp_wifi_ap_get_sta_list(&stationList);
    tcpip_adapter_get_sta_list(&stationList, &adapterList);

    String json = "{";
    json += "\"count\":" + String(adapterList.num) + ",";
    json += "\"clients\":[";

    for (int i = 0; i < adapterList.num; i++) {
        auto st = adapterList.sta[i];

        char macStr[18];
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
                st.mac[0], st.mac[1], st.mac[2],
                st.mac[3], st.mac[4], st.mac[5]);

        json += "{";
        json += "\"mac\":\"";
        json += macStr;
        json += "\",\"ip\":\"";
        json += ip4addr_ntoa((const ip4_addr_t*)&st.ip);
        json += "\"}";

        if (i < adapterList.num - 1) json += ",";
    }

    json += "]}";

    request->send(200, "application/json", json);
}

// -------------------------------------------------
// Setup WiFi Manager (Async)
// -------------------------------------------------

void wifiManagerSetup() {
    Serial.println("=== WiFi Manager Init ===");

    WiFi.scanNetworks(true);

    enableAP();

    String ssid, pass;
    if (loadWiFi(ssid, pass)) {
        WiFi.begin(ssid.c_str(), pass.c_str());

        unsigned long t0 = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - t0 < 8000) {
            delay(100);
        }
    }

    LittleFS.begin();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        String path = request->url();

        if (LittleFS.exists(path)) {
            request->send(LittleFS, path, String());
        } else {
            request->send(404, "text/plain", "Not found");
        }
    });

    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "RESETTING");
        delay(200);
        ESP.restart();
    });

    server.on("/clearwifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        clearWiFiCredentials();
        request->send(200, "text/plain", "WIFI_CLEARED");
        delay(300);
        ESP.restart();
    });

    server.on("/scan", HTTP_GET, handleScan);
    server.on("/connect", HTTP_GET, handleConnect);
    server.on("/status", HTTP_GET, handleStatus);
    server.on("/clients", HTTP_GET, handleClients);

    server.begin();
}

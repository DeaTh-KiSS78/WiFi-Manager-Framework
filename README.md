# ESP32 WiFi Manager Template

Template standard pentru toate proiectele mele ESP32.
Include:
- AsyncWebServer
- LittleFS
- AP fallback
- Scanare WiFi asincronă
- Conectare STA
- UI HTML/CSS/JS
- Reset WiFi
- Reset device
- Structură corectă PlatformIO

Structură:
- src/ → cod C++
- include/ → headere
- data/ → fișiere web (LittleFS)

Comenzi:
- pio run -t upload
- pio run -t uploadfs

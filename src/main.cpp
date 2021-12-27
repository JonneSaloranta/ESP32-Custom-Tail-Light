#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

const char* wifi_ssid = "test";
const char* wifi_password = "test";

const char* ap_ssid = "Custom-Tail-Light";
const char* ap_password = "12345678";

AsyncWebServer server(80);

void showFiles() {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while (file) {
        Serial.print("File: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
}

void connectToWiFi() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(wifi_ssid, wifi_password);

    Serial.print("Connecting to ");
    Serial.print(wifi_ssid);

    uint16_t retry = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (retry > 100) {
            Serial.print("Couldn't connect to ");
            Serial.println(wifi_ssid);
            return;
        }
        delay(100);
        Serial.print(".");
        retry++;
    }
    // Print wifi details if connected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Couldn't connect to WiFi.");
    } else {
        Serial.println();
        Serial.println("Connected!");
        Serial.print("IP address for network ");
        Serial.print(wifi_ssid);
        Serial.print(" : ");
        Serial.println(WiFi.localIP());
    }
}

void createAP() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ap_ssid, ap_password);

    // Print AP details
    Serial.print("IP address for network ");
    Serial.print(ap_ssid);
    Serial.print(" : ");
    Serial.print(WiFi.softAPIP());
    Serial.println();
}

void setup(void) {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("Error mounting SPIFFS");
        return;
    }

    showFiles();
    connectToWiFi();
    createAP();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", String(), false);
    });

    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/styles.css", "text/css");
    });

    AsyncElegantOTA.begin(&server);  // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {}
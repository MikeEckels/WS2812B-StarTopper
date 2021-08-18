#include "WifiManager.h"

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

WifiManager::WifiManager() {}

WifiManager::~WifiManager() {}

void WebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Device disconnected!\n", num);
        break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Device connected at %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        webSocket.sendTXT(num, "Connected");
    }
                         break;

    case WStype_TEXT:
        //Serial.printf("[%u] Received: %s\n", num, payload);

        if (payload[0] == '#') {
            String message = String((char*)(payload));
            message = message.substring(1);
            Serial.println("Received: " + message);

            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, message);

            String ssid = doc["ssid"];
            String password = doc["password"];
            String authentication = doc["authentication"];

            if (ssid.length() > 0 && password.length() > 0) {
                Serial.println("Clearing EEPROM");
                for (int i = 0; i < 512; ++i) {
                    EEPROM.write(i, 0);
                }

                // Storing credentials
                Serial.println("Writing EEPROM ssid:");
                Serial.print("Wrote: ");
                for (int i = 0; i < ssid.length(); ++i) {
                    EEPROM.write(i, ssid[i]);
                    Serial.print(ssid[i]);
                }
                Serial.println();
                Serial.println();

                Serial.println("Writing EEPROM password:");
                Serial.print("Wrote: ");
                for (int i = 0; i < password.length(); ++i) {
                    EEPROM.write(32 + i, password[i]);
                    Serial.print(password[i]);
                }
                Serial.println();
                Serial.println();

                Serial.println("Writing EEPROM authentication token:");
                Serial.print("Wrote: ");
                for (int i = 0; i < authentication.length(); ++i) {
                    EEPROM.write(64 + i, authentication[i]);
                    Serial.print(authentication[i]);
                }
                Serial.println();
                Serial.println();

                EEPROM.commit();
                delay(2000);
                Serial.println("Restarting...");
                delay(2000);

                ESP.restart();
                break;
            }
        }
    }
}

void notFound(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
}

void WifiManager::EraseEeprom() {
    EEPROM.begin(512);
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 512; ++i)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

void WifiManager::SetupAP(char* ssid, char* password) {
    WiFi.disconnect();
    delay(100);
    WiFi.softAP(ssid, password);
    LaunchAP();
    Serial.println("Server Started");
    webSocket.begin();
    webSocket.onEvent(WebSocketEvent);
}

String WifiManager::ReadEeprom() {
    EEPROM.begin(512);
    Serial.println("Reading EEPROM");

    for (int i = 0; i < 32; ++i) {
        ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(ssid);

    for (int i = 32; i < 64; ++i) {
        password += char(EEPROM.read(i));
    }
    Serial.print("Password: ");
    Serial.println(password);

    String auth_token = "";
    for (int i = 64; i < 100; ++i) {
        auth_token += char(EEPROM.read(i));
    }
    Serial.print("Auth Token: ");
    Serial.println(auth_token);

    return auth_token;
}

void WifiManager::UpdateServer() {
    webSocket.loop();
}

bool WifiManager::GetConnectedState() {
    if (TestConnection()) {
        Serial.println("Succesfully Connected!");
        return true;
    }
    else {
        Serial.println("Starting the access point");
        return false;
    }
}

void WifiManager::CreateAP() {
    server.on("/", [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", webpage);
        });

    server.on("/submit", HTTP_GET, [](AsyncWebServerRequest* request) {
        String message;
        message = "Credentials received by StarTopper";
        request->send(200, "text/plain", message);
        });

    server.onNotFound(notFound);
    server.begin();
}

void WifiManager::LaunchAP() {
    Serial.println();
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());
    CreateAP();
}

bool WifiManager::TestConnection() {
    int i = 0;
    Serial.println("Waiting for Wifi connection");

    WiFi.begin(&ssid[0], &password[0]);
    while (i < 20) {
        if (WiFi.status() == WL_CONNECTED) {
            return true;
        }

        delay(500);
        Serial.print("*");
        i++;
    }
    Serial.println();
    Serial.println("Connection timed out");
    return false;
}

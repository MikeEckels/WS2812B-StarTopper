#include "WifiManager.h"

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

WifiManager::WifiManager() : wifiResetPin(5) {}

WifiManager::WifiManager(unsigned char wifiRestPin) : wifiResetPin(wifiResetPin) {}

WifiManager::~WifiManager() {}

void WifiManager::Begin() {
    pinMode(this->wifiResetPin, INPUT_PULLUP);
    EEPROM.begin(512);

    WifiManager::CheckEraseButton();
    WifiManager::ReadEeprom();
}

bool WifiManager::Connect() {
    DEBUG_PRINTLN("Waiting for Wifi connection");

    WiFi.begin(&ssid[0], &password[0]);
    for (int i = 0; i < 20; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            DEBUG_PRINT_NOTICE("Succesfully Connected!");
            this->connectionState = true;
            return true;
        }

        delay(500);
        DEBUG_PRINT("*");
    }
    DEBUG_PRINTLN();
    DEBUG_PRINT_ERR("Connection timed out");
    DEBUG_PRINT_NOTICE("Starting the access point");
    this->connectionState = false;
    return false;
}

bool WifiManager::SpawnAP(char* ssid, char* password) {
    WiFi.disconnect();
    delay(100);
    bool state = WiFi.softAP(ssid, password);

    DEBUG_PRINTLN();
    if (state) {
        DEBUG_PRINTLN("Access Point IP: ");
        DEBUG_PRINT_INFO(WiFi.softAPIP());
        CreateAP();

        webSocket.begin();
        webSocket.onEvent(WebSocketEventHandler);
    }
    else {
        DEBUG_PRINT_ERR("Access Point failed to start");
    }

    return state;
}

void WifiManager::UpdateServer() {
    webSocket.loop();
}

bool WifiManager::GetConnectedState() {
    return this->connectionState;
}

String WifiManager::GetSSID() {
    return ssid;
}

String WifiManager::GetPassword() {
    return password;
}

String WifiManager::GetAuthentication() {
    return this->authentication;
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

    server.onNotFound(notFoundHandler);
    server.begin();

    DEBUG_PRINT_NOTICE("Server Started");
}

void WifiManager::ReadEeprom() {
    DEBUG_PRINT_NOTICE("Reading EEPROM");

    for (int i = 0; i < 32; ++i) {
        this->ssid += char(EEPROM.read(i));
        this->password += char(EEPROM.read(i + 32));
        this->authentication += char(EEPROM.read(i + 64));
    }
    DEBUG_PRINTLN("SSID: ");
    DEBUG_PRINT_INFO(this->ssid);
    DEBUG_PRINTLN("Password: ");
    DEBUG_PRINT_INFO(this->password);
    DEBUG_PRINTLN("Authentication: ");
    DEBUG_PRINT_INFO(this->authentication);
}

void WifiManager::EraseEeprom() {
    DEBUG_PRINT_NOTICE("Clearing EEPROM");
    for (int i = 0; i < 512; ++i) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

void WifiManager::CheckEraseButton() {
    delay(this->rstBtnDelaySec * 1000);

    if (!digitalRead(this->wifiResetPin)) {
        WifiManager::EraseEeprom();
    }
}

void WifiManager::notFoundHandler(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
}

void WifiManager::WebSocketEventHandler(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        DEBUG_PRINTF("[%u] Device disconnected!\n", num);
        break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        DEBUG_PRINTF("[%u] Device connected at %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        webSocket.sendTXT(num, "Connected");
    }
        break;

    case WStype_TEXT:
        if (payload[0] == '#') {
            String message = String((char*)(payload));
            message = message.substring(1);
           DEBUG_PRINT_NOTICE("Received: " + message);

            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, message);

            String ssid = doc["ssid"];
            String password = doc["password"];
            String authentication = doc["authentication"];

            if (ssid.length() > 0 && password.length() > 0) {
                DEBUG_PRINT_NOTICE("Clearing EEPROM");
                for (int i = 0; i < 512; ++i) {
                    EEPROM.write(i, 0);
                }

                // Storing credentials
                DEBUG_PRINT_NOTICE("Writing EEPROM ssid:");
                DEBUG_PRINTLN("Wrote: ");
                DEBUG_PRINT_INFO_NO_LN("");
                for (int i = 0; i < ssid.length(); ++i) {
                    EEPROM.write(i, ssid[i]);
                    DEBUG_PRINT(ssid[i]);
                }
                DEBUG_PRINTLN();
                DEBUG_PRINTLN();

                DEBUG_PRINT_NOTICE("Writing EEPROM password:");
                DEBUG_PRINTLN("Wrote: ");
                DEBUG_PRINT_INFO_NO_LN("");
                for (int i = 0; i < password.length(); ++i) {
                    EEPROM.write(32 + i, password[i]);
                    DEBUG_PRINT(password[i]);
                }
                DEBUG_PRINTLN();
                DEBUG_PRINTLN();

                DEBUG_PRINT_NOTICE("Writing EEPROM authentication token:");
                DEBUG_PRINTLN("Wrote: ");
                DEBUG_PRINT_INFO_NO_LN("");
                for (int i = 0; i < authentication.length(); ++i) {
                    EEPROM.write(64 + i, authentication[i]);
                    DEBUG_PRINT(authentication[i]);
                }
                DEBUG_PRINTLN();
                DEBUG_PRINTLN();

                EEPROM.commit();
                delay(2000);
                DEBUG_PRINT_NOTICE("Restarting...");
                delay(2000);

                ESP.restart();
                break;
            }
        }
    }
}

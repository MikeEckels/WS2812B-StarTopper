#include "OTAHandler.h"

void UpdateHandler::SetupOTA(const char* hostName, const char* ssid, const char* password) {
    ArduinoOTA.setHostname(hostName);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    //ArduinoOTA.setPort(3232);
    //ArduinoOTA.setPassword("password");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        Serial.println("Start updating " + type);
        });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("\nAuth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("\nBegin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("\nConnect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("\nReceive Failed");
        else if (error == OTA_END_ERROR) Serial.println("\nEnd Failed");
        });

    ArduinoOTA.begin();
    TelnetStream.begin();

    Serial.println("OTA Initialized");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void UpdateHandler::handle() {
    ArduinoOTA.handle();
}
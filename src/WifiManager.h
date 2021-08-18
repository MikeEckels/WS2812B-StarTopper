#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "Unsupported Board Type. Please use ESP8266 or ESP32"
#endif

#include "Arduino.h"
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "WifiManagerWebPage.h"

class WifiManager {
private:
	String ssid = "";
	String password = "";

	void CreateAP();
	void LaunchAP();
	bool TestConnection();

public:
	WifiManager();
	~WifiManager();

	void EraseEeprom();
	void SetupAP(char* softap_ssid, char* softap_pass);
	String ReadEeprom();
	void UpdateServer();

	bool GetConnectedState();
};

#endif //WIFI_MANAGER_H

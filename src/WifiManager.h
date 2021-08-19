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
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>

#include "DebugUtils.h"
#include "WifiManagerWebPage.h"


class WifiManager {
private:
	WifiManager(const WifiManager&) = delete;
	WifiManager& operator = (const WifiManager&) = delete;

	String ssid;
	String password;
	String authentication;
	bool connectionState = false;
	unsigned char rstBtnDelaySec = 4;
	unsigned char wifiResetPin = NULL;

	void CreateAP();
	void ReadEeprom();
	void EraseEeprom();
	void CheckEraseButton();

	static void notFoundHandler(AsyncWebServerRequest* request);
	static void WebSocketEventHandler(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
	
public:
	WifiManager();
	WifiManager(unsigned char wifiResetPin);
	~WifiManager();

	void Begin();
	bool Connect();
	bool SpawnAP(char* softap_ssid, char* softap_pass);
	void UpdateServer();

	String GetSSID();
	String GetPassword();
	String GetAuthentication();
	bool GetConnectedState();

};

#endif //WIFI_MANAGER_H

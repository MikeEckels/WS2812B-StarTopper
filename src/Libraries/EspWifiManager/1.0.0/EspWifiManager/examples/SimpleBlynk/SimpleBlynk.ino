/*	EspWifiManager SimpleBlynk Example
	Author: Mike Eckels
	Date: 08/23/2021

	Dependencies: 
	
	Purpose: This example shows how to use the EspWifiManager with Blynk credentials.
	The Esp will read the contents of its EEPROM and if Wi-Fi credentials are present
	it will attempt to connect and configure Blynk with an authentication token. If 
	the wifi connection fails, or no credentials are found in the EEPROM, it will
	automatically spawn an access point where the user can enter the network SSID,
	password, and Blynk authentication token. Upon submitting, the Esp will save the
	credentials to EEPROM and restart. The procedure then repeats*/

/*Determining what Blynk library to include.If embedding WifiManager into a classand using blynk,
be sure to uncomment and swap out both "BlynkSimpleEsp8266.h" and "BlynkSimpleEsp32.h", and add
"BlynkWifi Blynk(_blynkTransport);" to the global scope of your classes cpp file. (Blynk limitation)
*/
#ifdef ESP8266
#include <BlynkSimpleEsp8266.h>
//#include"BlynkSimpleEsp8266Fixed.h"
#elif defined(ESP32)
#include <BlynkSimpleEsp32.h>
//#include"BlynkSimpleEsp32Fixed.h"
#else
#error "Unsupported Board Type. Please use ESP8266 or ESP32"
#endif

#include <WifiManager.h>

/*The constructor can be default, or take in two pins :
		WifiResetPin: INPUT_PULLUP -- resets wifi credentials if pressed within first 4sec of startup
		RandomSeedPin: ANALOG INPUT -- used to generate a random seed for a unique SSID
*/

//WifiManager networkManager;
WifiManager networkManager = WifiManager(5, A0); //WifiManager(WifiResetPin, RandomSeedPin)

//Change this to desired AP SSID
String accessPointSSIDPrefix = "WifiManager"; 
//Change this to desired AP Password or leave blank 
String accessPointPass = "";
//Storage for Blynk auth token
char auth[33];

void setup() {
	//All debug messages will print over serial unless "DEBUG" is commented out in "DebugUtils.h"
	Serial.begin(115200);

	//Must be called before using the library
	networkManager.Begin();

	//Attempt to connect and return pass/fail result
	if (networkManager.Connect()) {

		//Get the auth token from the WifiManager and convert it to a char array for Blynk
		networkManager.GetAuthentication().toCharArray(auth, 33);

		//configure Blynk with the unique auth token
		Blynk.config(auth);
	}
	else {
		//Error no wifi
		//Attempt to spqwn accessPoint and return pass/fail result
		if (!networkManager.SpawnAP(accessPointSSIDPrefix, accessPointPass)) {
			//Error cant spawn AP
		}
	}
}

void loop() {
	//Must be continuously called in loop
	networkManager.UpdateServer();

	//If connected to Wi-Fi
	if (networkManager.GetConnectedState()) {

		//Run Blynk
		Blynk.run();

		//Run Loop code here
	}
}

#ifndef STAR_H
#define STAR_H

//#define DEV_TOPPER
//#define CHRIS_TOPPER
#define MOM_TOPPER

#ifdef DEV_TOPPER
#define BLYNK_TEMPLATE_ID "TMPLsSXg8uS6"
#define BLYNK_TEMPLATE_NAME "DevStarTopper"
#endif

#ifdef CHRIS_TOPPER
#define BLYNK_TEMPLATE_ID "TMPLsvLCbD74"
#define BLYNK_TEMPLATE_NAME "ChrisStarTopper"
#endif

#ifdef MOM_TOPPER
#define BLYNK_TEMPLATE_ID "TMPL2FI4gYD1A"
#define BLYNK_TEMPLATE_NAME "StarTopper"
#endif

#include "Arduino.h"

#ifdef ESP8266
//#include <BlynkSimpleEsp8266.h>
#include <BlynkSimpleEsp8266Fixed.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
#elif defined(ESP32)
//#include <BlynkSimpleEsp32.h>
#include <BlynkSimpleEsp32Fixed.h>
#else
#error "Unsupported Board Type. Please use ESP8266 or ESP32"
#endif

#include<functional>
#include <FastLED.h>
#include <WifiManager.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

static bool callBlynkOnce;
static WidgetTerminal terminal(V6);

static struct colorParams_t {
	unsigned int hue = 0;
	unsigned int red = 0;
	unsigned int blue = 0;
	unsigned int green = 0;
	unsigned int brightness = 20;
}colorParams;

class Star {
private:
	//WifiManager Network Definitions
	char auth[33];
	String espSSIDPrefix = "StarTopper";
	String espPass = "";
	static const unsigned char wifiResetPin = 5;
	static const unsigned char randomSeedPin = A0;
	WifiManager networkManager;// = WifiManager(wifiResetPin, randomSeedPin);

	//Hardware Definitions
	static const unsigned char pin = 4;
	static const EOrder colorOrder = GRB;
	static const LEDColorCorrection colorCorrection = TypicalSMD5050;
	unsigned int framesPerSecond = 120;

	//Array Sizes
	static const unsigned int numPoints = 5;
	static const unsigned int numLEDsTotal = 126;
	static const unsigned int numLEDsBorder = 42;
	static const unsigned int numLEDsPerPoint = 9;
	static const unsigned int numLEDsMiddle = (numLEDsTotal / 2) - (numLEDsPerPoint * numPoints);

	//LED Arrays
	CRGB leds[numLEDsTotal];
	const unsigned int ledBorder[numLEDsBorder] = {0, 1, 2, 4, 5, 6,
												   7, 8, 9, 10,11,12,
												   24,25,40,41,42,43,
												   44,45,46,45,48,49,
												   50,51,52,54,55,57,
												   58,59,60,61,62,32,
												   33,31,19,18,3, 1};

	const unsigned int ledMiddle[numLEDsMiddle] = {16,15,14,20,21,22,
												   23,30,29,28,27,26,
												   35,36,37,38,57,56};

	const unsigned int points[numPoints][numLEDsPerPoint] = {{0, 1, 2, 3, 4, 5, 18,17,19},
															 {6, 7, 8, 9, 10,11,12,13,24},
															 {25,39,40,41,42,43,44,45,46},
															 {48,49,50,51,52,53,54,55,47},
															 {31,32,33,34,58,59,60,61,62}};

	//Member Function Pointer Array
	typedef void (Star::* tpatternFn) ();
	tpatternFn patterns[9] = { &Star::TracePattern,    &Star::BouncePattern,    &Star::JugglePattern,
							   &Star::RainbowPattern,  &Star::GRainbowPattern,  &Star::ConfettiPattern,
							   &Star::PinwheelPattern, &Star::StarburstPattern, &Star::RasterizePattern };
	
	//Visual Parameters
	bool apError = false;
	bool patternMode = 0;
	unsigned int currentPattern = 0;

	void WifiError();
	void TracePattern();
	void BouncePattern();
	void JugglePattern();
	void RainbowPattern();
	void GRainbowPattern();
	void ConfettiPattern();
	void PinwheelPattern();
	void StarburstPattern();
	void RasterizePattern();
	void AddGlitter(fract8 chance);

	void WifiResetCallback(void);
	void WifiLoadingCallback(int status);

public:
	Star();
	void begin();
	void Update();
	void NextPattern();
	void LoopPatterns(unsigned int nSeconds);

	bool blynkConnected = false;
	bool disconnectError = false;
	
	bool GetMode();
	colorParams_t GetColors();

	void SetPattern(unsigned int patternNumber);
	void SetMode(bool isPattern);
	void SetColor(unsigned int r, unsigned int g, unsigned int b);
	void SetBrightness(unsigned int brightness);
	void SetFramesPerSecond(unsigned int fps);

};

#endif //STAR_H

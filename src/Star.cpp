#include "Star.h"

BlynkWifi Blynk(_blynkTransport);

namespace internStar {
	Star* pThis;
}

Star::Star() {
	internStar::pThis = this;
}

void Star::begin() {
	FastLED.addLeds<WS2812B, this->pin, this->colorOrder>(this->leds, this->numLEDsTotal).setCorrection(this->colorCorrection);
	networkManager.Begin(std::bind(&Star::WifiResetCallback, this));

	if (networkManager.Connect(std::bind(&Star::WifiLoadingCallback, this, std::placeholders::_1))) {
		this->apError = false;
		networkManager.GetAuthentication().toCharArray(this->auth, 33);
		Blynk.config(this->auth);

	}
	else {
		//Error no wifi
		if (!networkManager.SpawnAP(this->espSSIDPrefix, this->espPass)) {
			this->apError = true;
			//Error cant spawn AP
		}
	}
}

void Star::Update() {
	networkManager.UpdateServer();
	EVERY_N_SECONDS(2) { Star::WifiError(); }

	if (networkManager.GetConnectedState()) {
		Blynk.run();
		FastLED.show();
		if (this->patternMode) {
			(this->*patterns[this->currentPattern])();
			FastLED.show();
			FastLED.delay((1000 / this->framesPerSecond));
			EVERY_N_MILLISECONDS(20) { colorParams.hue++; }
		}
	}
}

void Star::NextPattern() {
	if (this->patternMode) {
		FastLED.clear();
		this->currentPattern = (this->currentPattern + 1) % ARRAY_SIZE(this->patterns);
	}
}

void Star::LoopPatterns(unsigned int nSeconds) {
	if (this->patternMode) {
		EVERY_N_SECONDS(nSeconds) { NextPattern(); }
	}
}

void Star::SetPattern(unsigned int patternNumber) {
	if (patternNumber >= ((ARRAY_SIZE(this->patterns)) - 1)) {
		patternNumber = ((ARRAY_SIZE(this->patterns)) - 1);
	}
	else if (patternNumber <= 0) {
		patternNumber = 0;
	}

	this->currentPattern = patternNumber;
}

void Star::SetMode(bool isPattern) {
	this->patternMode = isPattern;
}

void Star::SetColor(unsigned int r, unsigned int g, unsigned int b) {
	colorParams.red = r;
	colorParams.green = g;
	colorParams.blue = b;
	if (!(this->patternMode)) {
		for (unsigned int led = 0; led < this->numLEDsTotal; led++) {
			this->leds[led] = CRGB(r, g, b);
		}
	}
}

void Star::SetBrightness(unsigned int brightness) {
	colorParams.brightness = brightness;
	FastLED.setBrightness(brightness);
}

void Star::SetFramesPerSecond(unsigned int fps) {
	this->framesPerSecond = fps;
}

void Star::WifiError() {
	if (!networkManager.GetConnectedState()) {
		Star::SetMode(0);

		if (this->apError) {
			Star::SetColor(255, 255, 0);
		}
		else {
			Star::SetColor(255, 0, 0);
		}

		Star::SetBrightness(50);
		FastLED.show();
		delay(300);
		Star::SetBrightness(0);
		FastLED.show();
		delay(300);
		Star::SetBrightness(50);
		FastLED.show();
		delay(300);
		Star::SetBrightness(0);
		FastLED.show();
	}
}

void Star::TracePattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 20);
	int pos = beatsin16((this->framesPerSecond / 10), 0, (this->numLEDsBorder - 1));
	this->leds[this->ledBorder[pos]] += CHSV(colorParams.hue, 255, 192);
	this->leds[this->ledBorder[pos] + (this->numLEDsTotal / 2)] += CHSV(colorParams.hue, 255, 192);
}

void Star::BouncePattern() {
	uint8_t BeatsPerMinute = 62;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
	for (int i = 0; i < this->numLEDsTotal; i++) { //9948
		this->leds[i] = ColorFromPalette(palette, (colorParams.hue + (i * 2)), (beat - colorParams.hue + (i * 10)));
	}
}

void Star::JugglePattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 20);
	byte dothue = 0;
	for (int i = 0; i < 8; i++) {
		this->leds[beatsin16((i + 7), 0, (this->numLEDsTotal - 1))] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}

void Star::RainbowPattern() {
	fill_rainbow(this->leds, this->numLEDsTotal, colorParams.hue, 7);
}

void Star::GRainbowPattern() {
	RainbowPattern();
	AddGlitter(80);
}

void Star::ConfettiPattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 10);
	int pos = random16(this->numLEDsTotal);
	this->leds[pos] += CHSV((colorParams.hue + random8(64)), 200, 255);
}

void Star::PinwheelPattern() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < this->numLEDsPerPoint; j++) {
			this->leds[this->points[i][j]] += CHSV(colorParams.hue, 255, 192);
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] += CHSV(colorParams.hue, 255, 192);
			colorParams.hue++;
		}
		FastLED.show();
		FastLED.delay(this->framesPerSecond);
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < this->numLEDsPerPoint; j++) {
			this->leds[this->points[i][j]] = CRGB::Black;
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] = CRGB::Black;
		}
		FastLED.show();
		FastLED.delay(this->framesPerSecond);
	}
}

void Star::StarburstPattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 20);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < this->numLEDsPerPoint; j++) {
			this->leds[this->points[i][j]] += CHSV(colorParams.hue, 255, 192);
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] += CHSV(colorParams.hue, 255, 192);
			for (int k = 0; k < this->numLEDsMiddle; k++) {
				this->leds[this->ledMiddle[k]] += CHSV(colorParams.hue + 10, 255, 192);
				this->leds[this->ledMiddle[k] + (this->numLEDsTotal / 2)] += CHSV((colorParams.hue + 10), 255, 192);
			}
		}
	}
}

void Star::RasterizePattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 20);
	int pos = beatsin16((this->framesPerSecond / 10), 0, (this->numLEDsTotal - 1));
	this->leds[pos] += CHSV(colorParams.hue, 255, 192);
}

void Star::AddGlitter(fract8 chance) {
	if (random8() < chance) {
		this->leds[random16(this->numLEDsTotal)] += CRGB::White;
	}
}

void Star::WifiResetCallback(void) {
	Star::SetBrightness(50);
	for (unsigned char i = 0; i < 3; i++) {
		Star::SetColor(255, 0, 0);
		FastLED.show();
		delay(300);
		Star::SetColor(0, 0, 255);
		FastLED.show();
		delay(300);
	}
	Star::SetColor(0, 0, 0);
}

void Star::WifiLoadingCallback(int status) {
	static int i;
	static int oldStatus;
	static bool turningOn = true;

	DEBUG_PRINT(status);
	terminal.print(status);
	
	if (status != oldStatus) {
		i++;
		if (i >= this->numPoints) {
			i = 0;
			turningOn = !turningOn;
		}
	}

	if (turningOn) {
		for (int j = 0; j < this->numLEDsPerPoint; j++) {
			this->leds[this->points[i][j]] += CRGB(0, 0, 255);
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] += CRGB(0, 0, 255);
		}
	}
	else {
		for (int j = 0; j < this->numLEDsPerPoint; j++) {
			this->leds[this->points[i][j]] = CRGB::Black;
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] = CRGB::Black;
		}
	}

	FastLED.show();
	oldStatus = status;
}

bool Star::GetMode() {
	return this->patternMode;
}

colorParams_t Star::GetColors() {
	return colorParams;
}

extern BLYNK_CONNECTED() {	
	terminal.println("StarTopper Online");
	Blynk.virtualWrite(V1, internStar::pThis->GetMode());
	Blynk.virtualWrite(V2, internStar::pThis->GetColors().brightness);
	Blynk.virtualWrite(V3, internStar::pThis->GetColors().red);
	Blynk.virtualWrite(V4, internStar::pThis->GetColors().green);
	Blynk.virtualWrite(V5, internStar::pThis->GetColors().blue);
	Blynk.syncVirtual(V1, V2, V3, V4, V5);
}

extern BLYNK_WRITE(V1) {
	internStar::pThis->SetMode((bool)param.asInt());
	terminal.print("Mode set to: ");
	terminal.println((bool)param.asInt());
}

extern BLYNK_WRITE(V2) {
	internStar::pThis->SetBrightness((unsigned int)param.asInt());
	terminal.print("Brightness set to: ");
	terminal.println(param.asInt());
}

extern BLYNK_WRITE(V3) {
	internStar::pThis->SetColor((unsigned int)param.asInt(), internStar::pThis->GetColors().green, internStar::pThis->GetColors().blue);
	terminal.print("RED set to: ");
	terminal.println(param.asInt());
}

extern BLYNK_WRITE(V4) {
	internStar::pThis->SetColor(internStar::pThis->GetColors().red, (unsigned int)param.asInt(), internStar::pThis->GetColors().blue);
	terminal.print("GREEN set to: ");
	terminal.println(param.asInt());
}

extern BLYNK_WRITE(V5) {
	internStar::pThis->SetColor(internStar::pThis->GetColors().red, internStar::pThis->GetColors().green, (unsigned int)param.asInt());
	terminal.print("BLUE set to: ");
	terminal.println(param.asInt());
}

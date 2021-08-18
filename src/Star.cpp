#include "Star.h"

BlynkWifi Blynk(_blynkTransport);

namespace internStar {
	Star* pThis;
}

Star::Star() {
	internStar::pThis = this;
}

void Star::begin() {
	pinMode(this->pin, OUTPUT);
	pinMode(this->wifiResetPin, INPUT_PULLUP);

	delay(4000);
	if (!digitalRead(this->wifiResetPin)) {
		this->networkManager.EraseEeprom();
	}

	this->networkManager.ReadEeprom().toCharArray(this->authToken, 33);

	if (this->networkManager.GetConnectedState()) {
		Blynk.config(this->authToken);
		this->connected = 1;
	}
	else {
		this->networkManager.SetupAP(this->espSsid, this->espPass);
		this->connected = 0;
	}

	if (this->connected) {
		FastLED.addLeds<WS2812B, this->pin, this->colorOrder>(this->leds, this->numLEDsTotal).setCorrection(this->colorCorrection);
	}
}

void Star::Update() {
	this->networkManager.UpdateServer();

	if (this->connected) {
		Blynk.run();
		FastLED.show();
		if (this->patternMode) {
			(this->*patterns[this->currentPattern])();
			FastLED.show();
			FastLED.delay((1000 / this->framesPerSecond));
			EVERY_N_MILLISECONDS(20) { this->hue++; }
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
	if (!(this->patternMode)) {
		for (unsigned int led = 0; led < this->numLEDsTotal; led++) {
			this->leds[led] = CRGB(r, g, b);
		}
	}
}

void Star::SetBrightness(unsigned int brightness) {
	FastLED.setBrightness(brightness);
}

void Star::SetFramesPerSecond(unsigned int fps) {
	this->framesPerSecond = fps;
}

void Star::TracePattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 20);
	int pos = beatsin16((this->framesPerSecond / 10), 0, (this->numLEDsBorder - 1));
	this->leds[this->ledBorder[pos]] += CHSV(this->hue, 255, 192);
	this->leds[this->ledBorder[pos] + (this->numLEDsTotal / 2)] += CHSV(this->hue, 255, 192);
}

void Star::BouncePattern() {
	uint8_t BeatsPerMinute = 62;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
	for (int i = 0; i < this->numLEDsTotal; i++) { //9948
		this->leds[i] = ColorFromPalette(palette, (this->hue + (i * 2)), (beat - this->hue + (i * 10)));
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
	fill_rainbow(this->leds, this->numLEDsTotal, this->hue, 7);
}

void Star::GRainbowPattern() {
	RainbowPattern();
	AddGlitter(80);
}

void Star::ConfettiPattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 10);
	int pos = random16(this->numLEDsTotal);
	this->leds[pos] += CHSV((this->hue + random8(64)), 200, 255);
}

void Star::PinwheelPattern() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < this->numLEDsPerPoint; j++) {
			this->leds[this->points[i][j]] += CHSV(this->hue, 255, 192);
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] += CHSV(this->hue, 255, 192);
			this->hue++;
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
			this->leds[this->points[i][j]] += CHSV(this->hue, 255, 192);
			this->leds[this->points[i][j] + (this->numLEDsTotal / 2)] += CHSV(this->hue, 255, 192);
			for (int k = 0; k < this->numLEDsMiddle; k++) {
				this->leds[this->ledMiddle[k]] += CHSV(this->hue + 10, 255, 192);
				this->leds[this->ledMiddle[k] + (this->numLEDsTotal / 2)] += CHSV((this->hue + 10), 255, 192);
			}
		}
	}
}

void Star::RasterizePattern() {
	fadeToBlackBy(this->leds, this->numLEDsTotal, 20);
	int pos = beatsin16((this->framesPerSecond / 10), 0, (this->numLEDsTotal - 1));
	this->leds[pos] += CHSV(this->hue, 255, 192);
}

void Star::AddGlitter(fract8 chance) {
	if (random8() < chance) {
		this->leds[random16(this->numLEDsTotal)] += CRGB::White;
	}
}

extern BLYNK_WRITE(V1) {
	internStar::pThis->SetMode((bool)param.asInt());
}

extern BLYNK_WRITE(V2) {
	internStar::pThis->SetBrightness((unsigned int)param.asInt());
}

extern BLYNK_WRITE(V3) {
	internStar::pThis->SetColor((unsigned int)param[0].asInt(), (unsigned int)param[1].asInt(), (unsigned int)param[2].asInt());
}

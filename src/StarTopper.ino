#include "Star.h"

Star StarTopper;

void setup() {
	Serial.begin(115200);
	StarTopper.begin();

	/*Either set pattern mode with a brightness*/
	StarTopper.SetMode(1);
	StarTopper.SetBrightness(50);

	/*Or set color mode with a brightnessand colors*/
	//StarTopper.SetMode(0);
	//StarTopper.SetBrightness(50);
	//StarTopper.SetColor(255, 0, 70);

	StarTopper.SetFramesPerSecond(120);

	/*If in pattern mode you can chose a specific pattern to start on(0 - 8)
	Or repeat forever if not using "LoopPatterns()"*/
	//StarTopper.SetPattern(7);
}

void loop() {
	StarTopper.Update();
	StarTopper.LoopPatterns(10);
}

#include "Star.h"

Star StarTopper;

void setup() {
	Serial.begin(115200);
	StarTopper.begin();
	StarTopper.SetMode(1);
	StarTopper.SetFramesPerSecond(120);
	StarTopper.SetBrightness(50);
}

void loop() {
	StarTopper.Update();
	StarTopper.LoopPatterns(10);
}

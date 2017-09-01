// RotatingSix.h

#ifndef _ROTATINGSIX_h
#define _ROTATINGSIX_h

#include "Arduino.h"
#include "LEDAnimation.h"

#define MAX_TRACKS (6)

class RotatingLEDs
	: public IAnimation
{
private:
	AnimationTrack tracks[MAX_TRACKS];
	int count;

	int LEDDelay;
	int LEDOnTime;
	int LEDCycles;

	int nextLEDTime;
	int currentLED;
	int currentCycle;

	int face;
	int* ledIndices;
	int ledCount;

public:
	RotatingLEDs(int delay, int onTime, int cycles, int face, int leds[], int ledCount);
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], int retIntensities[]) override;
	virtual void clearLEDs() override;
	virtual int totalDuration() override;
};

#endif


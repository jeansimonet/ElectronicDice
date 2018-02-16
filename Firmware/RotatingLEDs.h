// RotatingSix.h

#ifndef _ROTATINGSIX_h
#define _ROTATINGSIX_h

#include "Arduino.h"
#include "IAnimation.h"
#include "Animation.h"

#define MAX_TRACKS (6)

/// <summary>
/// Blinks LEDs in a circle
/// </summary>
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

	RGBCurve rampUpDownCurve;

public:
	RotatingLEDs();
	RotatingLEDs(uint32_t color, int delay, int onTime, int cycles, int face, int leds[], int ledCount);
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], uint32_t retColors[]) override;
	virtual int stop(int retIndices[]) override;
	virtual int totalDuration() override;
};

#endif


// RandomLEDs.h

#ifndef _RANDOMLEDS_h
#define _RANDOMLEDS_h

#include "Arduino.h"
#include "LEDAnimation.h"

#define MAX_CONCURRENT_LEDS (16)

class RandomLEDs
	: public IAnimation
{
private:
	int duration;
	int ledOnTime;
	int delayBetweenLEDs;
	int nextLEDTime;

	struct LEDAndTime
	{
		int led;
		int startTime;
	};

	LEDAndTime leds[MAX_CONCURRENT_LEDS];
	int ledCount;

public:
	RandomLEDs(int duration, int ledOnTime, int delayBetweenLEDs);
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], int retIntensities[]) override;
	virtual void clearLEDs() override;
	virtual int totalDuration() override;
};

#endif


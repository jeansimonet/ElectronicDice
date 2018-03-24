// RandomLEDs.h

#ifndef _RANDOMLEDS_h
#define _RANDOMLEDS_h

#include "Arduino.h"
#include "IAnimation.h"

#define MAX_CONCURRENT_LEDS (16)

/// <summary>
/// Blinks the dice LEDs randomly
/// </summary>
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
		short led;
		short startTime;
	};

	LEDAndTime leds[MAX_CONCURRENT_LEDS];
	int colorIndices[MAX_CONCURRENT_LEDS];
	int ledCount;

public:
	RandomLEDs();
	RandomLEDs(int duration, int ledOnTime, int delayBetweenLEDs);
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], uint32_t retColors[]) override;
	virtual int stop(int retIndices[]) override;
	virtual int totalDuration() override;
};

#endif


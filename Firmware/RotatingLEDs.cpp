// 
// 
// 

#include "RotatingLEDs.h"
#include "LEDController.h"
#include "LEDAnimation.h"
#include "LEDAnimations.h"
#include "DiceLED.h"

RotatingLEDs::RotatingLEDs(int delay, int onTime, int cycles, int face, int leds[], int ledCount)
	: LEDDelay(delay)
	, LEDOnTime(onTime)
	, LEDCycles(cycles)
	, count(0)
	, nextLEDTime(0)
	, currentLED(-1)
	, currentCycle(0)
	, ledIndices( leds )
	, face(face)
	, ledCount(ledCount)
{
	
}

void RotatingLEDs::start()
{
	count = 0;
	nextLEDTime = 0;
	currentLED = -1;
	currentCycle = 0;
}

int RotatingLEDs::updateLEDs(int time, int retIndices[], int retIntensities[])
{
	if (time >= nextLEDTime)
	{
		bool addLED = true;
		currentLED++;
		if (currentLED == ledCount)
		{
			currentLED = 0;
			currentCycle++;
			if (currentCycle == LEDCycles)
			{
				// We're done! Let the animation finish
				nextLEDTime = 0x7FFFFFFF; // no more leds need to be turned on
				addLED = false;
			}
		}

		if (addLED)
		{
			// Spawn another led!
			auto& nextTrack = tracks[count];
			nextTrack.face = face;
			nextTrack.index = ledIndices[currentLED];
			nextTrack.startTime = nextLEDTime;	// ms
			nextTrack.duration = LEDOnTime;	// ms
			nextTrack.curve = &rampUpDown;
			count++;

			// Set the next LED time
			nextLEDTime += LEDDelay;
		}
	}

	// Now update the current leds
	int retCount = 0;
	for (int i = 0; i < count; ++i)
	{
		// Is the track expired?
		if (time >= tracks[i].startTime + tracks[i].duration)
		{
			// Tell ledcontroller to turn off the led
			retIndices[retCount] = LEDs.ledIndex(tracks[i].face, tracks[i].index);
			retIntensities[retCount] = 0;
			retCount++;

			// And remove the track
			for (int j = i; j < count - 1; ++j)
			{
				tracks[j] = tracks[j + 1];
			}
			count--;
			i--;
		}
		else
		{
			retIndices[retCount] = LEDs.ledIndex(tracks[i].face, tracks[i].index);
			retIntensities[retCount] = tracks[i].evaluate(time);
			retCount++;
		}
	}

	return retCount;
}

void RotatingLEDs::clearLEDs()
{
	int zeros[6] =
	{
		0,0,0,0,0,0,
	};
	
	ledController.setLEDs(ledIndices, zeros, ledCount);
}

int RotatingLEDs::totalDuration()
{
	return LEDCycles * LEDDelay * ledCount + LEDOnTime;
}

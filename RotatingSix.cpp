// 
// 
// 

#include "RotatingSix.h"
#include "LEDController.h"
#include "LEDAnimation.h"
#include "LEDAnimations.h"
#include "DiceLED.h"

RotatingSix::RotatingSix(int delay, int onTime, int cycles)
	: LEDDelay(delay)
	, LEDOnTime(onTime)
	, LEDCycles(cycles)
	, count(0)
	, nextLEDTime(0)
	, currentLED(-1)
	, currentCycle(0)
	, ledIndices{0, 1, 2, 5, 4, 3}
{
	
}

void RotatingSix::start()
{
	count = 0;
	nextLEDTime = 0;
	currentLED = -1;
	currentCycle = 0;
}

int RotatingSix::updateLEDs(int time, int retIndices[], int retIntensities[])
{
	if (time >= nextLEDTime)
	{
		bool addLED = true;
		currentLED++;
		if (currentLED == 6)
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
			nextTrack.face = 5;
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

void RotatingSix::clearLEDs()
{
	int indices[6] =
	{
		LEDs.ledIndex(5,0),
		LEDs.ledIndex(5,1),
		LEDs.ledIndex(5,2),
		LEDs.ledIndex(5,3),
		LEDs.ledIndex(5,4),
		LEDs.ledIndex(5,5),
	};

	int zeros[6] =
	{
		0,0,0,0,0,0,
	};
	
	ledController.setLEDs(indices, zeros, 6);
}

int RotatingSix::totalDuration()
{
	return LEDCycles * LEDDelay * 6 + LEDOnTime;
}

RotatingSix rotatingSix(100, 500, 5);
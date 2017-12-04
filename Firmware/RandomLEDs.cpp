// 
// 
// 

#include "RandomLEDs.h"
#include "LEDController.h"
#include "LEDAnimation.h"
#include "LEDAnimations.h"
#include "DiceLED.h"

RandomLEDs::RandomLEDs(int duration, int ledOnTime, int delayBetweenLEDs)
	: duration(duration)
	, ledOnTime(ledOnTime)
	, delayBetweenLEDs(delayBetweenLEDs)
	, ledCount(0)
{
}

void RandomLEDs::start()
{
	nextLEDTime = delayBetweenLEDs;
}

int RandomLEDs::updateLEDs(int time, int retIndices[], int retIntensities[])
{
	if (time < duration - ledOnTime && time >= nextLEDTime)
	{
		// Add a new led!
		if (ledCount < MAX_CONCURRENT_LEDS)
		{
			leds[ledCount].led = random(LED_COUNT);
			leds[ledCount].startTime = time;
			ledCount++;
		}
		nextLEDTime += delayBetweenLEDs;
	}

	// Now update the current leds
	int retCount = 0;
	for (int i = 0; i < ledCount; ++i)
	{
		// Is the track expired?
		if (time >= leds[i].startTime + ledOnTime)
		{
			// Tell ledcontroller to turn off the led
			retIndices[retCount] = leds[i].led;
			retIntensities[retCount] = 0;
			retCount++;

			// And remove the led from array
			for (int j = i; j < ledCount - 1; ++j)
			{
				leds[j] = leds[j + 1];
			}
			ledCount--;
			i--;
		}
		else
		{
			int scaledTime = (time - leds[i].startTime) * 256 / ledOnTime;
			retIndices[retCount] = leds[i].led;
			retIntensities[retCount] = rampUpDown.evaluate(scaledTime);
			retCount++;
		}
	}

	return retCount;
}

void RandomLEDs::clearLEDs()
{
	ledController.clearAll();
}

int RandomLEDs::totalDuration()
{
	return duration;
}

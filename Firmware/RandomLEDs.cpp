#include "RandomLEDs.h"
#include "IAnimation.h"
#include "LEDAnimations.h"
#include "LEDs.h"
#include "Utils.h"

using namespace Core;

uint32_t randomColors[] =
{
	toColor(255, 0, 0),
	toColor(0, 255, 0),
	toColor(0, 0, 255),
	toColor(255, 255, 0),
	toColor(0, 255, 255),
	toColor(255, 0, 255),
	toColor(255, 255, 255),
};

#define COLOR_COUNT 7

RandomLEDs::RandomLEDs()
	: duration(0)
	, ledOnTime(0)
	, delayBetweenLEDs(0)
	, ledCount(0)
{

}

/// <summary>
/// Initializing constructor
/// </summary>
/// <param name="duration">How long the animation should be</param>
/// <param name="ledOnTime">How long each led should stay on</param>
/// <param name="delayBetweenLEDs">How long to wait until we turn on the next one</param>
RandomLEDs::RandomLEDs(int duration, int ledOnTime, int delayBetweenLEDs)
	: duration(duration)
	, ledOnTime(ledOnTime)
	, delayBetweenLEDs(delayBetweenLEDs)
	, ledCount(0)
{
}

/// <summary>
/// Kick off the animation
/// </summary>
void RandomLEDs::start()
{
	nextLEDTime = delayBetweenLEDs;
}

/// <summary>
/// Computes the list of LEDs that need to be on, and what their intensities should be
/// based on the different tracks of this animation.
/// </summary>
/// <param name="time">The animation time (in milliseconds)</param>
/// <param name="retIndices">the return list of LED indices to fill, max size should be at least 21, the total number of leds</param>
/// <param name="retColors">the return list of LED intensities to fill, max size should be at least 21, the total number of leds</param>
/// <returns>The number of leds/intensities added to the return array</returns>
int RandomLEDs::updateLEDs(int time, int retIndices[], uint32_t retColors[])
{
	if (time < duration - ledOnTime && time >= nextLEDTime)
	{
		// Add a new led!
		if (ledCount < MAX_CONCURRENT_LEDS)
		{
			leds[ledCount].led = random(LED_COUNT);
			colorIndices[ledCount] = random(COLOR_COUNT);
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
			retColors[retCount] = 0;
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
			retColors[retCount] = LEDAnimations::rampUpDown.evaluate(randomColors[colorIndices[i]], scaledTime);
			retCount++;
		}
	}

	return retCount;
}

/// <summary>
/// Stop this animation
/// </summary>
int RandomLEDs::stop(int retIndices[])
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		retIndices[i] = i;
	}
	return LED_COUNT;
}

/// <summary>
/// How long is this animation?
/// </summary>
int RandomLEDs::totalDuration()
{
	return duration;
}

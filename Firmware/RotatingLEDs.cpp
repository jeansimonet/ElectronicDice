#include "RotatingLEDs.h"
#include "AnimController.h"
#include "LEDAnimations.h"
#include "LEDs.h"

/// <summary>
/// Default Constructor
/// </summary>
RotatingLEDs::RotatingLEDs()
	: LEDDelay(0)
	, LEDOnTime(0)
	, LEDCycles(0)
	, count(0)
	, nextLEDTime(0)
	, currentLED(-1)
	, currentCycle(0)
	, ledIndices(0)
	, face(0)
	, ledCount(0)
{
	rampUpDownCurve = RGBCurve::fromColorAndCurve(0, LEDAnimations::rampUpDown);
}

/// <summary>
/// Initializing constructor
/// </summary>
/// <param name="color">The color of the leds</param>
/// <param name="delay">Time between when each consecutive LED turns on (ms)</param>
/// <param name="onTime">How long each LED should stay on</param>
/// <param name="cycles">How many times each LED will end up lighting up</param>
/// <param name="face">Which face we're working with</param>
/// <param name="leds">Which leds on that face we're working with</param>
/// <param name="ledCount">size of the passed in array of leds to work with</param>
RotatingLEDs::RotatingLEDs(uint32_t color, int delay, int onTime, int cycles, int face, int leds[], int ledCount)
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
	rampUpDownCurve = RGBCurve::fromColorAndCurve(color, LEDAnimations::rampUpDown);
}

/// <summary>
/// Kick off the animation
/// </summary>
void RotatingLEDs::start()
{
	count = 0;
	nextLEDTime = 0;
	currentLED = -1;
	currentCycle = 0;
}

/// <summary>
/// Computes the list of LEDs that need to be on, and what their intensities should be
/// based on the different tracks of this animation.
/// </summary>
/// <param name="time">The animation time (in milliseconds)</param>
/// <param name="retIndices">the return list of LED indices to fill, max size should be at least 21, the total number of leds</param>
/// <param name="retIntensities">the return list of LED intensities to fill, max size should be at least 21, the total number of leds</param>
/// <returns>The number of leds/intensities added to the return array</returns>
int RotatingLEDs::updateLEDs(int time, int retIndices[], uint32_t retColors[])
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
			nextTrack.curve = &rampUpDownCurve;
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
			retIndices[retCount] = LEDs::ledIndex(tracks[i].face, tracks[i].index);
			retColors[retCount] = 0;
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
			retIndices[retCount] = LEDs::ledIndex(tracks[i].face, tracks[i].index);
			retColors[retCount] = tracks[i].evaluate(time);
			retCount++;
		}
	}

	return retCount;
}

/// <summary>
/// Stop this animation
/// </summary>
int RotatingLEDs::stop(int retIndices[])
{
	memcpy(retIndices, ledIndices, ledCount * sizeof(int));
	return ledCount;
}

/// <summary>
/// How long is this animation?
/// </summary>
int RotatingLEDs::totalDuration()
{
	return LEDCycles * LEDDelay * ledCount + LEDOnTime;
}

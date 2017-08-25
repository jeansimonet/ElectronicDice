// 
// 
// 

#include "LEDController.h"
#include "Dice_LED.h"
#include "Dice_Timer.h"
#include "Dice_Debug.h"

#define MAX_LEVEL (256)

// The average time it takes to send the led commands over I2C is about 256 us,
// meaning our minimum pulse width is 256 us.
// Because of that we can only accomodate a maximum intensity level of roughly 64
// times our minimum intensity (because 64 (intensity) * 256 (us) * 60 (fps) ~= 980 ms)
// So we'll just divide the requested levels by 4 (and round up so 1 turns the led on)
#define RESOLUTION (256)
#define STANDARD_DURATION (64) // This will give us 60 Hz led refresh

LEDController ledController;

LEDController::LEDIndexAndMarker::LEDIndexAndMarker()
	: count(0)
{
}

LEDController::LEDController()
	: currentLedIndex(0)
	, bufferIndex(0)
	, swapQueued(false)
{
	// Initialize the led intensities
	for (int i = 0; i < LED_COUNT; ++i)
	{
		allLedIntensities[i] = 0;
	}
}

LEDController::LEDIndexAndMarker& LEDController::getCurrentLEDsAndMarkers()
{
	return ledsAndMarkers[bufferIndex];
}

LEDController::LEDIndexAndMarker& LEDController::getNextLEDsAndMarkers()
{
	return ledsAndMarkers[1 - bufferIndex];
}

void LEDController::swapLEDsAndMarkers()
{
	bufferIndex = 1 - bufferIndex;
}

void LEDController::setLED(int index, int intensity)
{
	int prevIntensity = allLedIntensities[index];
	if (intensity != prevIntensity)
	{
		// First update the intensity array
		allLedIntensities[index] = intensity;

		// Then figure out how much we need to scale the pulses, such that our refresh rate is between 30 and 60 Hz
		// Scaling everything down means we have less diference between full-bright and almost off, so if we only
		// have a few leds on, we want to try and allow them to be on for as long as possible when full-bright.

		// Then compute the timer intervals
		int totalTimeOn = 0;
		int totalLeds = 0;

		// We use a double buffer to avoid changing the data the interrupt-drive update() method goes through
		auto& ledAndMarkers = getNextLEDsAndMarkers();

		// Iterate the led array and give all leds time intervals
		for (int i = 0; i < LED_COUNT; ++i)
		{
			int li = allLedIntensities[i];
			if (li > 0)
			{
				// Account for it in the total time leds are on
				// We'll need this to decide whether to scale the durations to keep a decent fps
				totalTimeOn += li;

				// Store the duration for this led
				ledAndMarkers.ledDurations[totalLeds] = li;
				ledAndMarkers.ledIndices[totalLeds] = i;
				totalLeds++;
			}
		}

		// We may want to add time with all leds off after the last led if most intensive led
		// is on for more time than it should to achieve its desired level
		if (totalTimeOn > 0)
		{
			// By default we scale all durations by 4, so that up to a certain intensity we don't
			// have to dim all the leds to accomodate.
			int scaleFactor = 4;

			// But we may need to scale down more than that to keep a decent fps
			if (totalTimeOn / scaleFactor > STANDARD_DURATION)
			{
				scaleFactor = totalTimeOn / STANDARD_DURATION;
			}

			// Add empty time if all leds are really dim and our fps would be too high
			if (totalTimeOn / scaleFactor < STANDARD_DURATION)
			{
				ledAndMarkers.ledDurations[totalLeds] = STANDARD_DURATION * scaleFactor - totalTimeOn;
				ledAndMarkers.ledIndices[totalLeds] = LED_COUNT; // Special meaning, means OFF!
				totalLeds++;
			}

			// Scale everything down
			for (int i = 0; i < totalLeds; ++i)
			{
				ledAndMarkers.ledDurations[i] = (ledAndMarkers.ledDurations[i] + scaleFactor - 1) / scaleFactor;
			}
		}
		else
		{
			// Turn leds off right away
			LEDs.clear();
		}

		ledAndMarkers.count = totalLeds;

		// Debug print timings!
		if (diceDebug.isDebugOn())
		{
			int ticks = 0;
			for (int i = 0; i < totalLeds; ++i)
			{
				diceDebug.print(ledAndMarkers.ledIndices[i]);
				diceDebug.print(": ");
				diceDebug.println(ledAndMarkers.ledDurations[i]);
				ticks += ledAndMarkers.ledDurations[i];
			}
			diceDebug.print("total: ");
			diceDebug.println(ticks);
		}

		// If we are not currently turning any leds on/off, then forcibly swap the led buffers
		if (ledAndMarkers.count > 0 && getCurrentLEDsAndMarkers().count == 0)
		{
			swapLEDsAndMarkers();
		}
		else
		{
			// Else we'll have the update method swap as soon as it's ready
			swapQueued = true;
		}
	}
	// Else nothing to change!
}

int LEDController::ledControllerUpdate()
{
	return ledController.update();
}

void LEDController::begin()
{
	diceTimer.hook(RESOLUTION, LEDController::ledControllerUpdate);
}

void LEDController::stop()
{
	diceTimer.unHook(LEDController::ledControllerUpdate);
}

int LEDController::update()
{
	digitalWrite(1, LOW);
	int ret = RESOLUTION * STANDARD_DURATION;
	auto& ledAndMarkers = getCurrentLEDsAndMarkers();
	if (ledAndMarkers.count > 0)
	{
		// Switch the led off / switch to the next led
		currentLedIndex++;
		if (currentLedIndex >= ledAndMarkers.count)
		{
			currentLedIndex = 0;

			// Swap buffers and restart
			if (swapQueued)
			{
				swapLEDsAndMarkers();
				swapQueued = false;
			}
			digitalWrite(1, HIGH);
		}
		else
		{
			digitalWrite(1, LOW);
		}

		// Flip the next led on or all leds off
		int nextLed = ledAndMarkers.ledIndices[currentLedIndex];
		if (nextLed == LED_COUNT)
		{
			// Turn all leds off
			LEDs.clear();
		}
		else
		{
			// Turn the next LED on
			LEDs.set(nextLed);
		}

		// Wait until next led on/off time
		int ledDuration = ledAndMarkers.ledDurations[currentLedIndex];
		ret = ledDuration * RESOLUTION;
	}

	return ret;
}

void LEDController::clearAll()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		allLedIntensities[i] = 0;
	}
	auto& ledsAndMarkers = getNextLEDsAndMarkers();
	for (int i = 0; i < LED_COUNT; ++i)
	{
		ledsAndMarkers.ledIndices[i] = 0;
		ledsAndMarkers.ledDurations[i] = 0;
	}
	ledsAndMarkers.count = 0;
}

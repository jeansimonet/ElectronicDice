#include "GPIOLEDController.h"
#include "GPIOLEDs.h"
#include "LEDs.h"
#include "Timer.h"
#include "Console.h"
#include "MessageQueue.h"

using namespace Core;
using namespace Systems;
using namespace Devices;

//#define DEBUG_LEVELS

#define MAX_LEVEL (256)

// The average time it takes to send the led commands over I2C is about 350us,
// meaning our minimum pulse width is 350us. To give ourselves some room to breathe
// We'll ask to be called by the timer every 500us or so (512 so that we have a nice
// round number).

// Assuming we want to try and keep a led refresh rate high enough to avoid flicker (~40 fps)
// it means that with ALL leds turned on, any single led can only be on for 1190 us.
// In other words, we could ramp the intensity of the entire Dice at once by just about 2.5 folds.

// Of course, if not all leds are on, then we can vary a lot more. A single LED could be on for
// a minimum of 512, and a maximum of 25,000us, or roughly 48 times brighter while maintaining
// a flicker-free refresh rate of ~40fps.

// But in order for led animations to not change the overall intensity too much, we'll
// set a default intensity scale such that a full intensity LED, even by itself is 16 times brighter
// than at its minimum level.

#define TIMER2_RESOLUTION (512)
#define MAX_LEVEL_TICKS (16)
#define STANDARD_DURATION (48)  // This will give us 40 Hz led refresh rate, and at that refresh rate
								// allow for 3 leds to be at full brightness

#define DIVISOR (MAX_LEVEL / MAX_LEVEL_TICKS)

/// <summary>
/// Constructor
/// </summary>
GPIOLEDController::LEDIndexAndMarker::LEDIndexAndMarker()
	: count(0)
{
}

/// <summary>
/// Constructor
/// </summary>
GPIOLEDController::GPIOLEDController(MessageQueue& queue)
	: messageQueue(queue)
	, currentLedIndex(0)
	, bufferIndex(0)
	, swapQueued(false)
	, ticks(0)
{
	// Initialize the led intensities
	for (int i = 0; i < LED_COUNT; ++i)
	{
		allLedIntensities[i] = 0;
	}
}

/// <summary>
/// Gets the array of LED indices currently being read by the interrupt routine
/// </summary>
GPIOLEDController::LEDIndexAndMarker* GPIOLEDController::getCurrentLEDsAndMarkers()
{
	return &(ledsAndMarkers[bufferIndex]);
}

/// <summary>
/// Gets the array of LED indices that we can write to (not being read by the interrupt routine)
/// </summary>
GPIOLEDController::LEDIndexAndMarker* GPIOLEDController::getNextLEDsAndMarkers()
{
	return &(ledsAndMarkers[1 - bufferIndex]);
}

/// <summary>
/// Indicate to the interrupt routine that it should swap the queues when it
/// is finished with the current one.
/// </summary>
void GPIOLEDController::queueSwap()
{
	swapQueued = true;
}

/// <summary>
/// Swap the lists of leds and time markers
/// </summary>
void GPIOLEDController::swapLEDsAndMarkers()
{
	bufferIndex = 1 - bufferIndex;
	swapQueued = false;
}

/// <summary>
/// Updates the led and time markers to accomodate the new
/// led and intensity being passed in.
/// </summary>
void GPIOLEDController::setLED(int index, int intensity)
{
	// First we look through the global list of led intensities
	// to see if the led was even on, and what intensity it was set to.
	int prevIntensity = allLedIntensities[index];
	if (intensity != prevIntensity)
	{
		// First update the intensity array
		allLedIntensities[index] = intensity;

		// Then recompute all current duration markers
		updateDurations();
	}
	// Else nothing to change!
}

/// <summary>
/// Sets an array of LEDs at once, similar to SetLED, but delays the
/// recomputation of the duration markers to the end.
/// </summary>
/// <param name="indices">The led indices to modify</param>
/// <param name="intensities">The matching intensities</param>
/// <param name="count">The number of leds passed in</param>
void GPIOLEDController::setLEDs(int indices[], int intensities[], int count)
{
	for (int i = 0; i < count; ++i)
	{
		allLedIntensities[indices[i]] = intensities[i];
	}
	updateDurations();
}

/// <summary>
/// Recompute the duration markers for the interrupt routine!
/// </summary>
void GPIOLEDController::updateDurations()
{
	int totalTimeOn = 0;
	int totalLeds = 0;

	// We use a double buffer to avoid changing the data the interrupt-drive update() method goes through
	auto ledAndMarkers = getNextLEDsAndMarkers();

	// Iterate the led array and set all leds markers based on led intensity
	for (int i = 0; i < LED_COUNT; ++i)
	{
		int li = (allLedIntensities[i] + DIVISOR - 1) / DIVISOR;
		if (li > 0)
		{
			// Store the duration for this led
			totalTimeOn += li;
			ledAndMarkers->ledMarkers[totalLeds] = totalTimeOn;
			ledAndMarkers->ledIndices[totalLeds] = i;
			totalLeds++;
		}
	}

	if (totalLeds > 0)
	{
		// We may want to add time with all leds off after the last led to maintain average perceived intensity
		if (totalTimeOn < STANDARD_DURATION)
		{
			ledAndMarkers->ledMarkers[totalLeds] = STANDARD_DURATION;
			ledAndMarkers->ledIndices[totalLeds] = LED_COUNT; // Special meaning, means OFF!
			totalLeds++;
		}

		ledAndMarkers->count = totalLeds;

		// Debug print timings!
#if DEBUG_LEVELS
		if (console->isDebugOn())
		{
			for (int i = 0; i < totalLeds; ++i)
			{
				console->print("[");
				console->print(ledAndMarkers->ledIndices[i]);
				console->print(":");
				console->print(ledAndMarkers->ledMarkers[i]);
				console->print("] ");
			}
			console->println(ticks);
		}
#endif

		queueSwap();
	}
	else
	{
		// We don't want any leds on, but did we already have all leds turned off?
		if (getCurrentLEDsAndMarkers()->count != 0)
		{
			// No, so queue a swap!
			ledAndMarkers->count = 0;
			queueSwap();
		}
	}
}

/// <summary>
/// Imterrupt routine
/// </summary>
void GPIOLEDController::ledControllerUpdate(void* param)
{
	((GPIOLEDController*)param)->update();
}

/// <summary>
/// Kick off the led controller, hooking it up to the timer system
/// </summary>
void GPIOLEDController::begin()
{
	timer.hook(TIMER2_RESOLUTION, GPIOLEDController::ledControllerUpdate, this);
}

/// <summary>
/// Stops the led controller
/// </summary>
void GPIOLEDController::stop()
{
	timer.unHook(GPIOLEDController::ledControllerUpdate);
}

/// <summary>
/// Interrupt routine, typically turns one LED on and another one off!
/// </summary>
void GPIOLEDController::update()
{
	auto currentLEDsAndMarkers = getCurrentLEDsAndMarkers();
	if (currentLEDsAndMarkers->count == 0 && swapQueued)
	{
		// Immediately swap the buffers
		swapLEDsAndMarkers();
		currentLEDsAndMarkers = getCurrentLEDsAndMarkers();
		ticks = -1;
		currentLedIndex = 0;
	}

	if (currentLEDsAndMarkers->count > 0)
	{
		// We need to know when our next switch is
		int currentLEDMarker = currentLEDsAndMarkers->ledMarkers[currentLedIndex];

		// Increment counter and compare, to see if we should switch a new led on
		ticks++;
		if (ticks == currentLEDMarker)
		{
			// We should switch to the next led
			currentLedIndex++;
			if (currentLedIndex == currentLEDsAndMarkers->count)
			{
				// Wrap around
				currentLedIndex = 0;

				// And take this opportunity to swap buffers
				if (swapQueued)
				{
					swapLEDsAndMarkers();
					currentLEDsAndMarkers = getCurrentLEDsAndMarkers();
				}

				// Reset the tick counter
				ticks = 0;
			}

			// Turn the led on/off
			if (currentLEDsAndMarkers->count > 0)
			{
				int nextLEDIndex = currentLEDsAndMarkers->ledIndices[currentLedIndex];
				if (nextLEDIndex < LED_COUNT)
				{
					// Switch to the next led
					MessageQueue::Message msg;
					msg.type = GPIO_MsgType_LEDOn;
					msg.intParam = nextLEDIndex;
					messageQueue.enqueue(msg);
				}
				else
				{
					// Special led index means stay off!
					MessageQueue::Message msg;
					msg.type = GPIO_MsgType_LEDsOff;
					messageQueue.enqueue(msg);
				}
			}
			else
			{
				// Clear all leds!
				MessageQueue::Message msg;
				msg.type = GPIO_MsgType_LEDsOff;
				messageQueue.enqueue(msg);
			}
		}
	}
	// Else there isn't much to do, we'll just wait until a swap is requested
}

/// <summary>
/// Clear all the LEDs
/// </summary>
void GPIOLEDController::clearAll()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		allLedIntensities[i] = 0;
	}
	auto ledsAndMarkers = getNextLEDsAndMarkers();
	for (int i = 0; i < LED_COUNT; ++i)
	{
		ledsAndMarkers->ledIndices[i] = 0;
		ledsAndMarkers->ledMarkers[i] = 0;
	}
	ledsAndMarkers->count = 0;
}

/// <summary>
/// Dumps the LED state to the console
/// </summary>
void GPIOLEDController::dumpToConsole()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		console.print(allLedIntensities[i]);
		console.print(", ");
	}
	console.println();
}

// Lazarus.h - Library for onBLE functions to break INFINITE waits in loop()
// original written by Thomas Olson Consulting

#include "Arduino.h"
#include "SimbleeBLE.h"
#include "Accelerometer.h"
#include "Lazarus.h"
#include "Die.h"

using namespace Systems;
using namespace Devices;

Lazarus Systems::lazarus;

#define radioPin 31
#define accelPin 23

#define AUTO_SLEEP_AFTER 10000 // 10s

/// <summary>
/// Constructor
/// </summary>
Lazarus::Lazarus()
{
	// Nothing for now
}

/// <summary>
/// Initialize the sleep/wake system
/// </summary>
void Lazarus::init()
{
	pinMode(radioPin, INPUT_PULLDOWN);
	NRF_GPIO->PIN_CNF[radioPin] = (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos); // use pulldown
	Simblee_resetPinWake(radioPin); // +++ <-- timb103 added this line

	// Set accelerometer interrupt pin as an input!
	pinMode(accelPin, INPUT_PULLUP);

	lastMillis = millis();

	die.RegisterUpdate(this, [](void* token) {((Lazarus*)token)->update(); });
}

/// <summary>
/// Called when we receive radio messages
/// </summary>
void Lazarus::onRadio()
{
	// Wake up the device!

	// Need to bring internal pin high somehow.. so..
	NRF_GPIO->PIN_CNF[radioPin] =
		(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
	Simblee_pinWake(radioPin, HIGH);
	// Need to bring internal pin low again.. so..
	NRF_GPIO->PIN_CNF[radioPin] =
		(GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);

	lastMillis = millis();
}

/// <summary>
/// Manually poke Lazarus so it doesn't go to sleep
/// </summary>
void Lazarus::poke()
{
	lastMillis = millis();
}

/// <summary>
/// Main update, called from die update()
/// </summary>
void Lazarus::update()
{
	// Very simple: if we haven't been poked by anyone in the last X many seconds we go to sleep!
	if (millis() - lastMillis > AUTO_SLEEP_AFTER)
	{
		// Go to sleep!
		sleepUntilInterrupt();

		// Refresh millis after wake up
		lastMillis = millis();
	}
}

/// <summary>
/// Puts the Simblee to sleep until the accelerometer detects an large enough interrupt
/// </summary>
void Lazarus::sleepUntilInterrupt()
{
	// Setup interrupt on accelerometer
	accelerometer.enableTransientInterrupt();

	// Prepare to wakeup on matching interrupt pin
	Simblee_pinWake(accelPin, LOW);

	// For the radio, we trigger on HIGH instead
	Simblee_pinWake(radioPin, HIGH);
									
	// Sleep forever
	Simblee_ULPDelay(INFINITE);

	// If we get here, we either got an accelerometer interrupt, or bluetooth message

	// Reset both pinwake flags
	if (Simblee_pinWoke(accelPin))
		Simblee_resetPinWake(accelPin);
	if (Simblee_pinWoke(radioPin))
		Simblee_resetPinWake(radioPin);

	// Disable accelerometer interrupts
	accelerometer.clearTransientInterrupt();
	accelerometer.disableTransientInterrupt();

	// Disable pinWake
	Simblee_pinWake(accelPin, DISABLE);
	Simblee_pinWake(radioPin, DISABLE);
}

/// <summary>
/// Stop wanting to put the die to sleep!
/// </summary>
void Lazarus::stop()
{
	die.UnregisterUpdateToken(this);
}
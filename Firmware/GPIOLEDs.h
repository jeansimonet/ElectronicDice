// Dice_LED.h
#ifndef _DICE_LED_h
#define _DICE_LED_h

#include "Arduino.h"

namespace Devices
{
#define LED_COUNT (21)

	/// <summary>
	/// Controls the LEDs on the Dice through a simple interface
	/// </summary>
	class GPIOLEDs
	{
	public:
		void init();
		void set(int face, int led);
		void set(int ledIndex);
		void clear();
		static int ledIndex(int face, int led);
	};
}

#endif


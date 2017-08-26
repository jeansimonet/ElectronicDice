// Dice_LED.h
#ifndef _DICE_LED_h
#define _DICE_LED_h

#include "Arduino.h"

#define LED_COUNT (21)

///
/// Controls the LEDs on the Dice through a simple interface
///
class DiceLEDs
{
public:
	void init();
	void set(int face, int led);
	void set(int ledIndex);
	void clear();
	int ledIndex(int face, int led);
};

extern DiceLEDs LEDs;

#endif


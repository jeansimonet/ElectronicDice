// DiceController.h

#ifndef _DICECONTROLLER_h
#define _DICECONTROLLER_h

#include "Arduino.h"

class DiceController
{
public:
	DiceController();

	void init();
	void sleepUntilInterrupt();
};

extern DiceController diceControl;

#endif


// 
// 
// 

#include "DiceController.h"
#include "DiceAccel.h"

#define int1pin 23

DiceController diceControl;

DiceController::DiceController()
{
	// Nothing for now...
}

void DiceController::init()
{
	// Set accelerometer interrupt pin as an input!
	pinMode(int1pin, INPUT_PULLUP);
}

// Puts the Simblee to sleep until the accelerometer detects an large enough interrupt
void DiceController::sleepUntilInterrupt()
{
	diceAccel.enableTransientInterrupt();
	Simblee_pinWake(int1pin, LOW);
	Simblee_ULPDelay(INFINITE);
	Simblee_resetPinWake(int1pin);
	diceAccel.clearTransientInterrupt();
	diceAccel.disableTransientInterrupt();
	Simblee_pinWake(int1pin, DISABLE);
}


// 
// 
// 

#include "AccelController.h"
#include "DiceTimer.h"
#include "DiceAccel.h"
#include "DiceWire.h"
#include "MessageQueue.h"
#include "DiceDebug.h"

#define TIMER2_RESOLUTION (100000) // 100ms


AccelController::AccelController()
	: face(0)
{
}

void AccelController::update()
{
	messageQueue.pushReadAccel(updateCurrentFace);
}

void AccelController::updateCurrentFace()
{
	accelController.face = accelController.determineFace(diceAccel.cx, diceAccel.cy, diceAccel.cz);
}

// To be passed to the timer
void AccelController::accelControllerUpdate()
{
	accelController.update();
}

void AccelController::begin()
{
	diceAccel.read();
	face = determineFace(diceAccel.cx, diceAccel.cy, diceAccel.cz);
	diceTimer.hook(TIMER2_RESOLUTION, AccelController::accelControllerUpdate);
}

void AccelController::stop()
{
	diceTimer.unHook(AccelController::accelControllerUpdate);
}

int AccelController::currentFace()
{
	return face;
}

int AccelController::determineFace(float x, float y, float z)
{
	if (abs(x) > abs(y))
	{
		if (abs(x) > abs(z))
		{
			// X is greatest direction
			if (x > 0)
			{
				return 4;
			}
			else
			{
				return 1;
			}
		}
		else
		{
			// Z is greatest direction
			if (z > 0)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}
	}
	else
	{
		if (abs(z) > abs(y))
		{
			// Z is greatest direction
			if (z > 0)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}
		else
		{
			// Y is greatest direction
			if (y > 0)
			{
				return 0;
			}
			else
			{
				return 5;
			}
		}
	}
}


AccelController accelController;

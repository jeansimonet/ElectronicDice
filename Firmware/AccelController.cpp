#include "AccelController.h"
#include "Timer.h"
#include "Accelerometer.h"
#include "Debug.h"

using namespace Systems;
using namespace Devices;

// This defines how frequently we try to read the accelerometer
#define TIMER2_RESOLUTION (10000)	// 10ms
#define JERK_SCALE (1000)		// To make the jerk in the same range as the acceleration

AccelerationController accelController;


/// <summary>
/// Concstructor
/// </summary>
AccelerationController::AccelerationController()
	: face(0)
{
}

/// <summary>
/// update is called from the timer
/// </summary>
void AccelerationController::timerUpdate()
{
	accelerometer.read();
	face = determineFace(accelerometer.cx, accelerometer.cy, accelerometer.cz);

	AccelFrame newFrame;
	newFrame.X = accelerometer.x;
	newFrame.Y = accelerometer.y;
	newFrame.Z = accelerometer.z;
	newFrame.Time = millis();

	// Compute delta!
	auto& lastFrame = buffer.last();
	short deltaX = newFrame.X - lastFrame.X;
	short deltaY = newFrame.Y - lastFrame.Y;
	short deltaZ = newFrame.Z - lastFrame.Z;

	// deltaTime should be roughly 10ms because that's how frequently we asked to be updated!
	short deltaTime = (short)(newFrame.Time - lastFrame.Time); 

	// Compute jerk
	// deltas are stored in the same unit (over time) as accelerometer readings
	// i.e. if readings are 8g scaled to a signed 12 bit integer (which they are)
	// then jerk is 8g/s scaled to a signed 12 bit integer
	newFrame.jerkX = deltaX * JERK_SCALE / deltaTime;
	newFrame.jerkY = deltaY * JERK_SCALE / deltaTime;
	newFrame.jerkZ = deltaZ * JERK_SCALE / deltaTime;

	buffer.push(newFrame);

	// Notify clients
	for (int i = 0; i < clientCount; ++i)
	{
		clients[i](newFrame);
	}
}

// To be passed to the timer
void AccelerationController::accelControllerUpdate(void* param)
{
	((AccelerationController*)param)->timerUpdate();
}

/// <summary>
/// Initialize the acceleration system
/// </summary>
void AccelerationController::begin()
{
	accelerometer.read();
	face = determineFace(accelerometer.cx, accelerometer.cy, accelerometer.cz);
	timer.hook(TIMER2_RESOLUTION, AccelerationController::accelControllerUpdate, this);
}

/// <summary>
/// Stop getting updated from the timer
/// </summary>
void AccelerationController::stop()
{
	timer.unHook(AccelerationController::accelControllerUpdate);
}

/// <summary>
/// Returns the currently stored up face!
/// </summary>
int AccelerationController::currentFace()
{
	return face;
}

/// <summary>
/// Crudely compares accelerometer readings passed in to determine the current face up
/// </summary>
/// <returns>The face number, starting at 0</returns>
int AccelerationController::determineFace(float x, float y, float z)
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

/// <summary>
/// Method used by clients to request timer callbacks when accelerometer readings are in
/// </summary>
void AccelerationController::hook(AccelerationController::ClientMethod callback)
{
	if (clientCount < MAX_CLIENTS)
	{
		clients[clientCount] = callback;
		clientCount++;
	}
	else
	{
		debugPrintln("Too many accelerometer hooks registered.");
	}
}

/// <summary>
/// Method used by clients to stop getting accelerometer reading callbacks
/// </summary>
void AccelerationController::unHook(AccelerationController::ClientMethod callback)
{
	int clientIndex = 0;
	for (; clientIndex < 4; ++clientIndex)
	{
		if (clients[clientIndex] == callback)
		{
			break;
		}
	}

	if (clientIndex != 4)
	{
		// Clear the entry
		clients[clientIndex] = nullptr;

		// Shift entries down
		for (; clientIndex < clientCount - 1; ++clientIndex)
		{
			clients[clientIndex] = clients[clientIndex + 1];
		}

		// Decrement total count
		clientCount--;
	}
	else
	{
		debugPrintln("Accelerometer hook was not found in the list of registered hooks.");
	}
}


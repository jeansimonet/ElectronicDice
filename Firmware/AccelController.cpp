#include "AccelController.h"
#include "Timer.h"
#include "Accelerometer.h"
#include "MessageQueue.h"

using namespace Core;
using namespace Systems;
using namespace Devices;

// This defines how frequently we try to read the accelerometer
#define TIMER2_RESOLUTION (10000) // 10ms

/// <summary>
/// Constructor
/// </summary>
AccelFrameQueue::AccelFrameQueue()
	: _count(0)
{
}

/// <summary>
/// How many samples do we have stored right now?
/// </summary>
int AccelFrameQueue::count() const
{
	return _count;
}

/// <summary>
/// Pushes a frame of accelerometer data onto the queue
/// </summary>
void AccelFrameQueue::push(short time, short x, short y, short z)
{
	if (_count == ACCEL_MAX_SIZE)
	{
		// Shift everything left
		for (int i = 0; i < ACCEL_MAX_SIZE - 1; ++i)
		{
			data[i] = data[i + 1];
		}
		_count--;
	}
	// Else do nothing

	// Add the new frame of data
	data[_count] = { x, y, z, time };
	_count++;
}

/// <summary>
/// Attempts to pop a frame of accelerometer data from the queue
/// </summary>
/// <param name="outFrame">The frame data that will receive the data</param>
/// <returns>true if a frame of data was pop'ed</returns>
bool AccelFrameQueue::tryPop(AccelFrame& outFrame)
{
	bool ret = _count > 0;
	if (ret)
	{
		outFrame = data[0];
		// Shift everything left
		for (int i = 0; i < _count - 1; ++i)
		{
			data[i] = data[i + 1];
		}
		_count--;
	}
	return ret;
}

/// <summary>
/// Concstructor
/// </summary>
AccelerationController::AccelerationController(MessageQueue& queue)
	: messageQueue(queue)
	, face(0)
{
}

/// <summary>
/// update is called from the timer
/// </summary>
void AccelerationController::update()
{
	Accelerometer::pushUpdateAccel(messageQueue);
	AccelerationController::pushUpdateFace(messageQueue);
}

/// <summary>
/// Perform some conversion and filtering work after reading the accelerometer values
/// </summary>
void AccelerationController::updateCurrentFace()
{
	face = determineFace(accelerometer.cx, accelerometer.cy, accelerometer.cz);
	queue.push((short)millis(), accelerometer.x, accelerometer.y, accelerometer.z);
}

// To be passed to the timer
void AccelerationController::accelControllerUpdate(void* param)
{
	((AccelerationController*)param)->update();
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
/// How many frames of data do we have stored?
/// </summary>
int AccelerationController::frameCount()
{
	return queue.count();
}

/// <summary>
/// Attempts to pop a frame of accelerometer data from the queue
/// </summary>
/// <param name="outFrame">The frame data that will receive the data</param>
/// <returns>true if a frame of data was pop'ed</returns>
bool AccelerationController::tryPop(AccelFrame& outFrame)
{
	return queue.tryPop(outFrame);
}

/// <summary>
/// Helper that queues up a message to update the controller's face data
/// </summary>
bool AccelerationController::pushUpdateFace(MessageQueue& queue)
{
	MessageQueue::Message mes;
	mes.type = MessageType_UpdateFace;
	return queue.enqueue(mes);
}
// AccelController.h

#ifndef _ACCELCONTROLLER_h
#define _ACCELCONTROLLER_h

#include "Arduino.h"

#define ACCEL_MAX_SIZE 32

// Forwards
namespace Core
{
	class MessageQueue;
}

/// <summary>
/// Small struct holding a single frame of accelerometer data
/// used for both face detection (not that kind) and telemetry
/// </summary>
struct AccelFrame
{
	short X;
	short Y;
	short Z;
	unsigned long Time;
};


/// <summary>
/// A small buffer of acceleraion readings, FIFO
/// </summary>
class AccelFrameQueue
{
public:
	AccelFrameQueue();

	int count() const;
	void push(short time, short x, short y, short z);
	bool tryPop(AccelFrame& outFrame);

private:
	AccelFrame data[ACCEL_MAX_SIZE];
	int _count;
};

/// <summary>
/// The component in charge of maintaining the acceleraion readings,
/// and determining die motion state.
/// </summary>
class AccelerationController
{
private:
	Core::MessageQueue& messageQueue;
	int face;

	// This is a small FIFO to buffer accelerometer readings
	// between the timed accel controller update, and variable-timed
	// main loop bluetooth message queue thingamajig! :)
	AccelFrameQueue queue;

private:
	// To be passed to the timer
	static void accelControllerUpdate(void* param);
	int determineFace(float x, float y, float z);

public:
	AccelerationController(Core::MessageQueue& queue);
	void begin();
	void stop();

	void update();
	void updateCurrentFace();
	int currentFace();

	int frameCount();
	bool tryPop(AccelFrame& outFrame);

#define MessageType_UpdateFace 4
	static bool pushUpdateFace(Core::MessageQueue& queue);
};
#endif


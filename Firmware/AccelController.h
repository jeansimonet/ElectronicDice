// AccelController.h

#ifndef _ACCELCONTROLLER_h
#define _ACCELCONTROLLER_h

#include "Arduino.h"
#include "RingBuffer.h"

#define ACCEL_BUFFER_SIZE 100 // 10ms * 100 = 1 seconds of buffer
							  // 16 bytes * 128 = 2k of RAM
#define MAX_CLIENTS 4

/// <summary>
/// Small struct holding a single frame of accelerometer data
/// used for both face detection (not that kind) and telemetry
/// </summary>
struct AccelFrame
{
	short X;
	short Y;
	short Z;
	short jerkX;
	short jerkY;
	short jerkZ;
	unsigned long Time;
};

/// <summary>
/// The component in charge of maintaining the acceleraion readings,
/// and determining die motion state.
/// </summary>
class AccelerationController
{
private:
	typedef void(*ClientMethod)(const AccelFrame& accelFrame);

	int face;

	// This small buffer stores about 1 second of Acceleration data
	Core::RingBuffer<AccelFrame, ACCEL_BUFFER_SIZE> buffer;

	// Clients needing to be notified when accelerometer readings come in
	ClientMethod clients[MAX_CLIENTS];
	int clientCount;

private:
	// To be passed to the timer
	static void accelControllerUpdate(void* param);
	int determineFace(float x, float y, float z);

public:
	AccelerationController();
	void begin();
	void stop();

	void timerUpdate();
	int currentFace();

	// Notification management
	void hook(ClientMethod method);
	void unHook(ClientMethod client);
};

extern AccelerationController accelController;

#endif


#ifndef _TELEMETRY_h
#define _TELEMETRY_h

#include "Arduino.h"
#include "AccelController.h"
#include "BluetoothMessage.h"

#define ACCEL_MAX_SIZE 8

namespace Systems
{
	/// <summary>
	/// Sends telemetry data to the app
	/// </summary>
	class Telemetry
	{
	private:
		DieMessageAcc teleMessage;
		AccelFrame lastAccelFrame;
		bool lastAccelWasSent;

	private:
		static void accelControllerCallback(void* ignore, const AccelFrame& frame);

	public:
		void begin();
		void onAccelFrame(const AccelFrame& frame);
	};

	extern Telemetry telemetry;
}
#endif


#ifndef _SLEEPMONITOR_h
#define _SLEEPMONITOR_h

#include "Arduino.h"
#include "AccelController.h"

namespace Systems
{
	/// <summary>
	/// Attaches to the accel controller and monitors jerk.
	/// if the jerk is high enough after a little bit of filtering,
	/// poke Lazarus so the Die doesn't go to sleep.
	/// </summary>
	class JerkMonitor
	{
	private:
		AccelFrame lastFrame;
		float sigma;

	private:
		static void accelControllerCallback(const AccelFrame& frame);

	public:
		void begin();
		void onAccelFrame(const AccelFrame& frame);
	};

	extern JerkMonitor jerkMonitor;
}

#endif


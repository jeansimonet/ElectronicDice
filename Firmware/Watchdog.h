// Watchdog.h

#ifndef _WATCHDOG_h
#define _WATCHDOG_h

#include "arduino.h"

namespace Systems
{
	class Watchdog
	{
	private:
		void sleepUntilInterrupt();
		static void MagnetInterrupt();

	public:
		Watchdog();
		void init();
		void update();
		void stop();
	};

	extern Watchdog watchdog;
}

#endif


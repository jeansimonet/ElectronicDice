/* Modified by Jean Simonet - Systemic Games */

/* Lazarus.h - Library for onBLE functions to break INFINITE waits in loop()
   by Thomas Olson Consulting
   20140806.01
*/

#ifndef Lazarus_h
#define Lazarus_h

namespace Systems
{
	class Lazarus
	{
	public:
		Lazarus();
		void init();
		void onRadio();
		void sleepUntilInterrupt();
	};

	extern Lazarus lazarus;
}

#endif

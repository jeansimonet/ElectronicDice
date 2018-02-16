// Dice_Timer.h

#ifndef _DICE_TIMER_h
#define _DICE_TIMER_h

#include "Arduino.h"

namespace Systems
{
	#define MAX_CLIENTS (4) 

	/// <summary>
	/// The timer class allows systems to request a callback on
	/// a periodic interval.
	/// </summary>
	struct Timer
	{
	public:
		typedef void(*ClientMethod)(void* param);

		void hook(int resolutionInMicroSeconds, ClientMethod client, void* param);
		void unHook(ClientMethod client);

		struct Client
		{
			ClientMethod callback;
			void* param;
			int ticks;
		};

	private:
		Client clients[MAX_CLIENTS];
		int count;

	private:
		static void timer2Interrupt();
		void update();

	public:
		Timer();
		void begin();
		void stop();
	};

	extern Timer timer;
}

#endif


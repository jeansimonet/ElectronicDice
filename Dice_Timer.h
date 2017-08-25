// Dice_Timer.h

#ifndef _DICE_TIMER_h
#define _DICE_TIMER_h

#include "Arduino.h"

#define MAX_CLIENTS (4) 

struct DiceTimer
{
public:
	typedef int(*ClientMethod)();

	void hook(int resolutionInMicroSeconds, ClientMethod client);
	void unHook(ClientMethod client);

	struct Client
	{
		ClientMethod callback;
		int ticks;
	};

private:
	Client clients[MAX_CLIENTS];
	int count;

private:
	static void timer2Interrupt();
	void update();

public:
	DiceTimer();
	void begin();
	void stop();
};

extern DiceTimer diceTimer;

#endif


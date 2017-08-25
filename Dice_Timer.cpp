// 
// 
// 

#include "Dice_Timer.h"
#include "Dice_Debug.h"

DiceTimer diceTimer;

#define RESOLUTION (16)

void DiceTimer::hook(int resolutionInMicroSeconds, DiceTimer::ClientMethod client)
{
	if (count < 4)
	{
		auto& clientInfo = clients[count];
		clientInfo.callback = client;
		clientInfo.ticks = resolutionInMicroSeconds / RESOLUTION;
		NRF_TIMER2->CC[count] = clientInfo.ticks;
		NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << (TIMER_INTENSET_COMPARE0_Pos + count);
		NRF_TIMER2->SHORTS |= (TIMER_SHORTS_COMPARE0_STOP_Enabled << (TIMER_SHORTS_COMPARE0_STOP_Pos + count));
		count++;
	}
	else
	{
		diceDebug.println("Too many timer hooks registered.");
	}
}

void DiceTimer::unHook(DiceTimer::ClientMethod client)
{
	int clientIndex = 0;
	for (; clientIndex < 4; ++clientIndex)
	{
		if (clients[clientIndex].callback == client)
		{
			break;
		}
	}

	if (clientIndex != 4)
	{
		// Clear the entry and timer entry
		NRF_TIMER2->CC[clientIndex] = 0;
		auto& clientInfo = clients[clientIndex];
		clientInfo.callback = nullptr;
		clientInfo.ticks = 0;

		// Shift entries down
		for (; clientIndex < count-1; ++clientIndex)
		{
			clients[clientIndex] = clients[clientIndex + 1];
			NRF_TIMER2->CC[clientIndex] = NRF_TIMER2->CC[clientIndex + 1];
		}

		// Decrement total count
		count--;

		// Disable compare event for the last client
		NRF_TIMER2->INTENCLR = TIMER_INTENSET_COMPARE0_Enabled << (TIMER_INTENSET_COMPARE0_Pos + count);  // taken from Nordic dev zone
	}
	else
	{
		diceDebug.println("Timer hook was not found in the list of registered hooks.");
	}
}

void DiceTimer::timer2Interrupt()
{
	diceTimer.update();
}

DiceTimer::DiceTimer()
	: count(0)
{
	NRF_TIMER2->TASKS_STOP = 1;	// Stop timer
	NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;  // taken from Nordic dev zone
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
	NRF_TIMER2->PRESCALER = 8;	// 16MHz / (2^8) = 16 us resolution
	NRF_TIMER2->TASKS_CLEAR = 1; // Clear timer
	NVIC_SetPriority(TIMER2_IRQn, 3);
	dynamic_attachInterrupt(TIMER2_IRQn, DiceTimer::timer2Interrupt);
}

void DiceTimer::begin()
{
	NRF_TIMER2->TASKS_START = 1;	// Start TIMER
}

void DiceTimer::stop()
{
	NRF_TIMER2->TASKS_STOP = 1;	// Stop timer
}

void DiceTimer::update()
{
	digitalWrite(0, HIGH);
	for (int i = 0; i < count; ++i)
	{
		if (NRF_TIMER2->EVENTS_COMPARE[i] != 0)
		{
			auto& clientInfo = clients[i];
			if (clientInfo.callback != nullptr)
			{
				// Trigger the callback and update the compare value for next multiple
				NRF_TIMER2->CC[i] = 0xFFFF & (NRF_TIMER2->CC[i] + clientInfo.callback() / RESOLUTION);

				// Clear interrupt
				NRF_TIMER2->EVENTS_COMPARE[i] = 0;
			}
			else
			{
				diceDebug.print("Timer event ");
				diceDebug.print(i);
				diceDebug.print(" does not have a registered hook!");
			}
		}
	}
	NRF_TIMER2->TASKS_START = 1;
	digitalWrite(0, LOW);
}
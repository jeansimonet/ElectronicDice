 // Messages.h

#ifndef _MESSAGES_h
#define _MESSAGES_h

#include "Arduino.h"

#define MESSAGE_MAX_COUNT 4

namespace Core
{
	/// <summary>
	/// Fast message queue used to delay device-dependent operations until the
	/// next iteration of the main loop. Because interacting with devices often
	/// requires using the Wire library (which internally relies on interrupts)
	/// we can't talk to the devices from inside interrupt themselves, including
	/// the timer interrupt that controls most of the die's behavior.
	/// </summary>
	class MessageQueue
	{
	public:
		struct Message
		{
		public:
			int type;
			union
			{
				// Used by SetName command
				char stringParam[16];

				// Used by SetLED command
				struct
				{
					uint32_t colorParam;
					int intParam;
				};

				// Used by SetLEDs command
				struct
				{
					int* indices;
					uint32_t* colors;
					int count;
				};
			};
		};

	private:
		Message messages[MESSAGE_MAX_COUNT];
		int count;
		int reader;
		int writer;

	public:
		MessageQueue();
		void init();
		bool enqueue(const Message& message);
		bool tryDequeue(Message& outMessage);
	};
}

#endif


// Messages.h

#ifndef _MESSAGES_h
#define _MESSAGES_h

#include "Arduino.h"

#define MESSAGE_MAX_COUNT 4

class MessageQueue
{
public:
	enum MessageType
	{
		MessageType_None = 0,
		MessageType_SetLED,
		MessageType_ClearLEDs,
		MessageType_ReadAccel
	};

	struct Message
	{
	public:
		MessageType type;
		int intParam;
		void(*callback)();
	};

private:
	Message messages[MESSAGE_MAX_COUNT];
	//Message currentMessage;
	int count;
	int reader;
	int writer;

public:
	MessageQueue();
	void init();
	bool pushSetLED(int ledIndex);
	bool pushClearLEDs();
	bool pushReadAccel(void(*callback)());
	void update();

private:
	bool enqueue(const Message& message);
	bool tryDequeue(Message& outMessage);
};

extern MessageQueue messageQueue;

#endif


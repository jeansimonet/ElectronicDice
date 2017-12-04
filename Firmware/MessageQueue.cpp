// 
// 
// 

#include "MessageQueue.h"
#include "DiceAccel.h"
#include "DiceLED.h"
#include "DiceDebug.h"

MessageQueue messageQueue;

MessageQueue::MessageQueue()
{
	init();
}

void MessageQueue::init()
{
	count = 0;
	reader = 0;
	writer = 0;
}

bool MessageQueue::pushSetLED(int ledIndex)
{
	Message mes;
	mes.type = MessageType_SetLED;
	mes.intParam = ledIndex;
	return enqueue(mes);
}

bool MessageQueue::pushClearLEDs()
{
	Message mes;
	mes.type = MessageType_ClearLEDs;
	return enqueue(mes);
}

bool MessageQueue::pushReadAccel(void(*callback)())
{
	Message mes;
	mes.type = MessageType_ReadAccel;
	mes.callback = callback;
	return enqueue(mes);
}

void MessageQueue::update()
{
	Message mes;
	while (tryDequeue(mes))
	{
		switch (mes.type)
		{
		case MessageType_SetLED:
			LEDs.set(mes.intParam);
			break;
		case MessageType_ClearLEDs:
			LEDs.clear();
			break;
		case MessageType_ReadAccel:
			diceAccel.read();
			mes.callback();
			break;
		default:
			break;
		}
	}
}

bool MessageQueue::enqueue(const Message& message)
{
	noInterrupts();
	bool ret = count < MESSAGE_MAX_COUNT;
	if (ret)
	{
		messages[writer] = message;
		writer = (writer + 1) % MESSAGE_MAX_COUNT;
		count++;
	}
	interrupts();
}

bool MessageQueue::tryDequeue(Message& outMessage)
{
	noInterrupts();
	bool ret = count > 0;
	if (ret)
	{
		outMessage = messages[reader];
		reader = (reader + 1) % MESSAGE_MAX_COUNT;
		count--;
	}
	interrupts();
	return ret;
}

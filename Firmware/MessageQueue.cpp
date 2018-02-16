#include "MessageQueue.h"

using namespace Core;

/// <summary>
/// Constructor
/// </summary>
MessageQueue::MessageQueue()
{
	init();
}

/// <summary>
/// Initialize the message queue
/// </summary>
void MessageQueue::init()
{
	count = 0;
	reader = 0;
	writer = 0;
}

/// <summary>
/// Internal queueing method
/// </summary>
/// <returns>true if the message was queued, false if the queue was full</returns>
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

/// <summary>
/// Internal dequeueing method
/// </summary>
/// <returns>true if a message was dequeued</returns>
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

#include "LEDs.h"
#include "Utils.h"

using namespace Core;

LEDs leds;

#define LED_COUNT (21)

#define MsgType_SetLED 1
#define MsgType_SetLEDs 2
#define MsgType_SetAll 3
#define MsgType_ClearAll 4

LEDs::LEDs()
#if !defined(RGB_LED)
	: controller(messageQueue)
#endif
{
}

void LEDs::init()
{
#if defined(RGB_LED)
	RGBLeds.init();
#else
	GPIOLeds.init();
	controller.begin();
#endif
}

void LEDs::update()
{
	MessageQueue::Message msg;
	while (messageQueue.tryDequeue(msg))
	{
		switch (msg.type)
		{
		case MsgType_SetLED:
			setLEDNow(msg.intParam, msg.colorParam);
			break;
		case MsgType_SetLEDs:
			setLEDsNow(msg.indices, msg.colors, msg.count);
			break;
		case MsgType_SetAll:
			setAllNow(msg.colorParam);
			break;
		case MsgType_ClearAll:
			clearAllNow();
			break;
#if !defined(RGB_LED)
		case GPIO_MsgType_LEDOn:
			GPIOLeds.set(msg.intParam);
			break;
		case GPIO_MsgType_LEDsOff:
			GPIOLeds.clear();
			break;
#endif
		}
	}
}

void LEDs::stop()
{
#if !defined(RGB_LED)
	controller.stop();
#endif
}

void LEDs::setLEDNow(int face, int led, uint32_t color)
{
	setLEDNow(ledIndex(face, led), color);
}

void LEDs::setLEDNow(int index, uint32_t color)
{
#if defined(RGB_LED)
	RGBLeds.Set(index, color);
#else
	controller.setLED(index, getGreyscale(color));
#endif
}

void LEDs::setLEDsNow(int indices[], uint32_t colors[], int count)
{
#if defined(RGB_LED)
	for (int i = 0; i < count; ++i)
	{
		RGBLeds.Set(indices[i], colors[i], false);
	}
	RGBLeds.show();
#else
	int intensities[LED_COUNT];
	for (int i = 0; i < count; ++i)
	{
		intensities[i] = getGreyscale(colors[i]);
	}
	controller.setLEDs(indices, intensities, count);
#endif
}

void LEDs::setAllNow(uint32_t color)
{
#if defined(RGB_LED)
	for (int i = 0; i < count; ++i)
	{
		RGBLeds.Set(i, color, false);
	}
	RGBLeds.show();
#else
	int indices[LED_COUNT];
	int intensities[LED_COUNT];
	byte greyscale = getGreyscale(color);
	for (int i = 0; i < LED_COUNT; ++i)
	{
		indices[i] = i;
		intensities[i] = getGreyscale(greyscale);
	}
	controller.setLEDs(indices, intensities, LED_COUNT);
#endif
}

void LEDs::clearAllNow()
{
#if defined(RGB_LED)
	RGBLeds.clear();
#else
	controller.clearAll();
#endif
}

void LEDs::setLED(int face, int led, uint32_t color)
{
	MessageQueue::Message msg;
	msg.type = MsgType_SetLED;
	msg.intParam = ledIndex(face, led);
	msg.colorParam = color;
	messageQueue.enqueue(msg);
}

void LEDs::setLED(int index, uint32_t color)
{
	MessageQueue::Message msg;
	msg.type = MsgType_SetLED;
	msg.intParam = index;
	msg.colorParam = color;
	messageQueue.enqueue(msg);
}

void LEDs::setLEDs(int indices[], uint32_t colors[], int count)
{
	// If a previous queued message was already using the buffers, Oh well... 
	memcpy(queuedIndices, indices, count * sizeof(int));
	memcpy(queuedColors, colors, count * sizeof(uint32_t));

	MessageQueue::Message msg;
	msg.type = MsgType_SetLEDs;
	msg.indices = queuedIndices;
	msg.colors = queuedColors;
	msg.count = count;
	messageQueue.enqueue(msg);
}

void LEDs::setAll(uint32_t color)
{
	MessageQueue::Message msg;
	msg.type = MsgType_SetAll;
	msg.colorParam = color;
	messageQueue.enqueue(msg);
}

void LEDs::clearAll()
{
	MessageQueue::Message msg;
	msg.type = MsgType_ClearAll;
	messageQueue.enqueue(msg);
}

int LEDs::ledIndex(int face, int led)
{
#if defined(RGB_LED)
	return APA102LEDs::ledIndex(face, led);
#else
	return Devices::GPIOLEDs::ledIndex(face, led);
#endif
}

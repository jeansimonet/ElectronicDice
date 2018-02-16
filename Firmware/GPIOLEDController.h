// LEDController.h

#ifndef _LEDCONTROLLER_h
#define _LEDCONTROLLER_h

#include "Arduino.h"

#define LED_COUNT (21)

// Forwards
namespace Core
{
	class MessageQueue;
}

/// <summary>
/// Turns LEDs on and off really fast to take advantage of persistence of vision
/// to simulate multiple LEDs on at once and varying levels of intensity.
/// The controller uses a double buffer to modify the timing values when adjusting
/// on/off time of each led, in order to try and minimize unwanted flickering.
/// </summary>
class GPIOLEDController
{
private:
	struct LEDIndexAndMarker
	{
	public:
		LEDIndexAndMarker();
		int ledIndices[LED_COUNT + 1];
		int ledMarkers[LED_COUNT + 1];
		int count;
	};

	byte allLedIntensities[LED_COUNT]; // Represents the current led intensities
	LEDIndexAndMarker ledsAndMarkers[2];

	int ticks;
	int currentLedIndex;
	int bufferIndex;
	bool swapQueued;

private:
	Core::MessageQueue& messageQueue;
	LEDIndexAndMarker * getCurrentLEDsAndMarkers();
	LEDIndexAndMarker* getNextLEDsAndMarkers();
	void queueSwap();
	void swapLEDsAndMarkers();
	void updateDurations();
	void update();

	// To be passed to the timer
	static void ledControllerUpdate(void* param);

public:
	GPIOLEDController(Core::MessageQueue& queue);
	void begin();
	void stop();

	void setLED(int index, int intensity); // Index 0 - 20, intensity 0 - 255
	void setLEDs(int indices[], int intensities[], int count);
	void clearAll();

	void dumpToConsole();
};

#endif


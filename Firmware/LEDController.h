// LEDController.h

#ifndef _LEDCONTROLLER_h
#define _LEDCONTROLLER_h

#include "Arduino.h"

#define LED_COUNT (21)

class LEDController
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
	LEDIndexAndMarker* getCurrentLEDsAndMarkers();
	LEDIndexAndMarker* getNextLEDsAndMarkers();
	void queueSwap();
	void swapLEDsAndMarkers();
	void updateDurations();
	void update();

	// To be passed to the timer
	static void ledControllerUpdate();

public:
	LEDController();
	void begin();
	void stop();

	void setLED(int index, int intensity); // Index 0 - 20, intensity 0 - 255
	void setLEDs(int indices[], int intensities[], int count);
	void clearAll();

	void printAllLeds();
};

extern LEDController ledController;

#endif


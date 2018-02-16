// DiceSettings.h

#ifndef _DICESETTINGS_h
#define _DICESETTINGS_h

#include "Arduino.h"

/// <summary>
/// Describes unique settings for this die
/// </summary>
class Settings
{
public:
	Settings();

	void init();

	void readSettings();
	void writeSettings();

	char name[16];

	// Test
	int someOtherData;
	float someFloatData;
};

#endif


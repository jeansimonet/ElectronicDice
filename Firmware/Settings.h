// DiceSettings.h

#ifndef _DICESETTINGS_h
#define _DICESETTINGS_h

#include "Arduino.h"

#define SETTINGS_PAGE (251)
#define SETTINGS_VALID_KEY (0x05E77165) // 0SETTINGS in leet speak ;)
#define SETTINGS_ADDRESS (SETTINGS_PAGE * 1024)

class Settings
{
public:
	// Indicates whether there is valid data
	uint32_t headMarker;
	char name[16];
	uint32_t tailMarker;

	bool CheckValid() const;
};

extern const Settings* settings;

#endif


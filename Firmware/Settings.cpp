#include "Settings.h"


const Settings* settings = (const Settings*)SETTINGS_ADDRESS;

bool Settings::CheckValid() const
{
	return headMarker == SETTINGS_VALID_KEY && tailMarker == SETTINGS_VALID_KEY;
}

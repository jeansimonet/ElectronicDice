#include "Settings.h"


Settings settings;

/// <summary>
/// Constructor
/// </summary>
Settings::Settings()
{
	char defaultName[] = "DefaultName";
	char* dst = name;
	char* src = defaultName;
	while (*dst++ = *src++);

	someOtherData = 55;
	//someFloatData = 1.0f;
}

/// <summary>
/// Intialize the settings
/// </summary>
void Settings::init()
{
	readSettings();
}

/// <summary>
/// Read the settings from EEPROM
/// </summary>
void Settings::readSettings()
{
	//// Check if this works!
	//// EEPROM.readBuffer(0, reinterpret_cast<byte*>(this), sizeof(DiceSettings));
	//int addr = 0;
	//eeprom.readString(addr, name, 16);
	//addr += sizeof(name);
	//someOtherData = eeprom.readInt(addr);
	//addr += sizeof(someOtherData);
	//someFloatData = eeprom.readFloat(addr);
	//addr += sizeof(someFloatData);
}

/// <summary>
/// Write the settings to EEPROM
/// </summary>
void Settings::writeSettings()
{
	//// Check if this works!
	//// EEPROM.writeBuffer(0, reinterpret_cast<byte*>(this), sizeof(DiceSettings));
	//int addr = 0;
	//eeprom.writeString(addr, name, 16);
	//addr += sizeof(name);
	//eeprom.writeInt(addr, someOtherData);
	//addr += sizeof(someOtherData);
	//eeprom.writeFloat(addr, someFloatData);
	//addr += sizeof(someFloatData);
}

// DiceEEPROM.h

#ifndef _DICEEEPROM_h
#define _DICEEEPROM_h
#include "Arduino.h"

namespace Devices
{
	class EEPROM
	{
	public:
		EEPROM();

		bool init();

		void writeByte(unsigned int eeaddress, byte data);
		void writeBuffer(unsigned int eeaddresspage, byte* data, byte length);

		byte readByte(unsigned int eeaddress);
		bool readByteTimeout(unsigned int eeaddress, int timeOut, byte& outByte);
		void readBuffer(unsigned int eeaddress, byte *buffer, int length);

		void writeInt(unsigned int eeaddress, int data);
		int readInt(unsigned int eeaddress);

		void writeFloat(unsigned int eeaddress, float data);
		float readFloat(unsigned int eeaddress);

		void writeString(unsigned int eeaddress, char* data, byte length);
		void readString(unsigned int eeaddress, char* buffer, int length);

		void dumpToConsole();
	private:
		uint32_t endOfLastWriteMillis;
	};

	extern EEPROM eeprom;
}

#endif


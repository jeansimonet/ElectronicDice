#include "EEPROM.h"
#include "I2C.h"
#include "Console.h"

using namespace Devices;
using namespace Systems;

EEPROM Devices::eeprom;

#define BASE_ADDRESS 0x50
#define LINE_SIZE 16
#define PAGE_SIZE 0xFF
#define PAGE_COUNT 2
#define DEVADDRESS_FROM_ADDRESS(x) (((x) >= PAGE_SIZE) ? BASE_ADDRESS : (BASE_ADDRESS + 1))
#define EEADDRESS_FROM_ADDRESS(x) ((x) % PAGE_SIZE)

/// <summary>
/// Constructor
/// </summary>
EEPROM::EEPROM()
{
	// Nothing for now...
}

/// <summary>
/// Initialize the EEPROM 
/// </summary>
bool EEPROM::init()
{
	// There is nothing special to do to the device itself!
	endOfLastWriteMillis = 0;

	byte ignore;
	return readByteTimeout(0, 100, ignore);
}

/// <summary>
/// Writes a byte of data to the EEPROM at a specific address
/// </summary>
void EEPROM::writeByte(unsigned int eeaddress, byte data)
{
	// Forcibly wait if too soon to try and write to the EEPROM
	int32_t wait = endOfLastWriteMillis + 4 - millis();
	if (wait > 0)
		delay(wait);

	// Write
	wire.beginTransmission(DEVADDRESS_FROM_ADDRESS(eeaddress));
	wire.write(EEADDRESS_FROM_ADDRESS(eeaddress)); // LSB
	wire.write(data);
	wire.endTransmission();

	// Remember when we last wrote!
	endOfLastWriteMillis = millis();
}

/// <summary>
/// Writes a buffer of data to the EEPROM, at a specific address, taking care to
/// split the operation if the data would cross page boundary.
/// Also, data can be maximum of about 30 bytes, because the Wire library
/// has a buffer of 32 bytes.
/// </summary>
void EEPROM::writeBuffer(unsigned int eeaddresspage, byte* data, byte length)
{
	// Forcibly wait if too soon to try and write to the EEPROM
	int32_t wait = endOfLastWriteMillis + 4 - millis();
	if (wait > 0)
		delay(wait);

	// Write
	auto devAddress = DEVADDRESS_FROM_ADDRESS(eeaddresspage);
	auto eeaadress = EEADDRESS_FROM_ADDRESS(eeaddresspage);
	unsigned int pageOffset = eeaadress % LINE_SIZE;
	if (pageOffset + length > LINE_SIZE)
	{
		// We'll need to split the buffer write!

		// First half!
		wire.beginTransmission(devAddress);
		wire.write(eeaddresspage); // LSB
		byte c;
		for (c = 0; c < LINE_SIZE - pageOffset; c++)
			wire.write(data[c]);
		wire.endTransmission();

		// Second half
		wire.beginTransmission(devAddress);
		wire.write(eeaddresspage + 1); // LSB
		for (c = LINE_SIZE - pageOffset; c < length; c++)
			wire.write(data[c]);
		wire.endTransmission();
	}
	else
	{
		// Write all bytes at once!
		wire.beginTransmission(devAddress);
		wire.write(eeaddresspage); // LSB
		byte c;
		for (c = 0; c < length; c++)
			wire.write(data[c]);
		wire.endTransmission();
	}

	// Remember when we last wrote!
	endOfLastWriteMillis = millis();
}

/// <summary>
/// Reads a byte of data from the EEPROM
/// </summary>
byte EEPROM::readByte(unsigned int eeaddress)
{
	byte rdata = 0xFF;
	wire.beginTransmission(DEVADDRESS_FROM_ADDRESS(eeaddress));
	wire.write(EEADDRESS_FROM_ADDRESS(eeaddress)); // LSB
	wire.endTransmission(false);
	wire.requestFrom(DEVADDRESS_FROM_ADDRESS(eeaddress), 1);
	while (!wire.available());
	rdata = wire.read();

	// Remember when we last read from eeprom!
	endOfLastWriteMillis = millis();

	return rdata;
}

/// <summary>
/// Reads a byte of data from the EEPROM
/// </summary>
bool EEPROM::readByteTimeout(unsigned int eeaddress, int timeOut, byte& outByte)
{
	wire.beginTransmission(DEVADDRESS_FROM_ADDRESS(eeaddress));
	wire.write(EEADDRESS_FROM_ADDRESS(eeaddress)); // LSB
	wire.endTransmission(false);
	wire.requestFrom(DEVADDRESS_FROM_ADDRESS(eeaddress), 1);

	bool ret = true;
	int start = millis();
	while (!wire.available())
	{
		if (millis() - start > timeOut)
		{
			// Took too long!!!!
			ret = false;
			break;
		}
	}

	if (ret)
	{
		// Go ahead and grab the data
		outByte = wire.read();

		// Remember when we last read from eeprom!
		endOfLastWriteMillis = millis();
	}

	return ret;
}

/// <summary>
/// Reads a buffer of data from the EEPROM
/// maybe let's not read more than 30 or 32 bytes at a time!
/// </summary>
void EEPROM::readBuffer(unsigned int eeaddress, byte *buffer, int length)
{
	// Forcibly wait if too soon to try and write to the EEPROM
	int32_t wait = endOfLastWriteMillis + 4 - millis();
	if (wait > 0)
		delay(wait);

	wire.beginTransmission(DEVADDRESS_FROM_ADDRESS(eeaddress));
	wire.write(EEADDRESS_FROM_ADDRESS(eeaddress)); // LSB
	wire.endTransmission(false);
	wire.requestFrom(DEVADDRESS_FROM_ADDRESS(eeaddress), length);
	int c = 0;
	for (c = 0; c < length; c++)
	{
		while (!wire.available());
		buffer[c] = wire.read();
	}

	// Remember when we last read from eeprom!
	endOfLastWriteMillis = millis();
}

/// <summary>
/// Helper method to write an int to the EEPROM
/// </summary>
void EEPROM::writeInt(unsigned int eeaddress, int data)
{
	writeBuffer(eeaddress, reinterpret_cast<byte*>(&data), sizeof(int));
}

/// <summary>
/// Helper method to read an int from the EEPROM
/// </summary>
int EEPROM::readInt(unsigned int eeaddress)
{
	int ret = 0;
	readBuffer(eeaddress, reinterpret_cast<byte*>(&ret), sizeof(int));
	return ret;
}

/// <summary>
/// Helper method to write a float to the EEPROM
/// </summary>
void EEPROM::writeFloat(unsigned int eeaddress, float data)
{
	writeBuffer(eeaddress, reinterpret_cast<byte*>(&data), sizeof(float));
}

/// <summary>
/// Helper method to read a float from the EEPROM
/// </summary>
float EEPROM::readFloat(unsigned int eeaddress)
{
	float ret = 0.0f;
	readBuffer(eeaddress, reinterpret_cast<byte*>(&ret), sizeof(float));
	return ret;
}

/// <summary>
/// Helper to write a string to the EEPROM. The string isn't expected to
/// be null terminated, so really this is more 'write a char array'!
/// </summary>
void EEPROM::writeString(unsigned int eeaddress, char* data, byte length)
{
	writeBuffer(eeaddress, reinterpret_cast<byte*>(data), length);
}

/// <summary>
/// Helper to read a string from the EEPROM. The string isn't expected to
/// be null terminated, so really this is more 'read a char array'!
/// </summary>
void EEPROM::readString(unsigned int eeaddress, char* buffer, int length)
{
	readBuffer(eeaddress, reinterpret_cast<byte*>(buffer), length);
}

/// <summary>
/// Dump the entire content of the EEPROM to console
/// </summary>
void EEPROM::dumpToConsole()
{
	int addr = 0;
	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			byte data = readByte(addr + j);
			char temp[4];
			sprintf(temp, "%02x", data);
			console.print(temp);
			console.print(" ");
		}
		console.print(" ");

		for (int j = 0; j < 16; ++j)
		{
			byte data = readByte(addr + j);
			if (data != '\n')
				console.print((char)data);
			else
				console.print(".");
		}
		console.println();
		addr += 32;
	}
}

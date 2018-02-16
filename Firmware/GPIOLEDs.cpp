#include "GPIOLEDs.h"
#include "I2C.h"
#include "Console.h"

using namespace Devices;
using namespace Systems;

#define DEV_ADRESS  0x70  // Address of the IO expander!

#define PTR_INPUT_REG   0  // read only
#define PTR_OUTPUT_REG  1  // rw
#define PTR_POL_REG     2  // rw
#define PTR_CONFIG_REG  3  // rw

/// <summary>
/// Internal LED representation
/// </summary>
struct LED
{
	byte HighMask;	// What pin needs to be high, as a mask
	byte LowMask;	// What pin needs to be low, as a mask
};

/// <summary>
/// Define high/low pins for each LED, as per the schematics of the die
/// </summary>
LED Leds[] =
{
	// Face 1
	{ 1 << 6, 1 << 7 },
	// Face 2
	{ 1 << 5, 1 << 4 },
	{ 1 << 4, 1 << 5 },
	// Face 3
	{ 1 << 7, 1 << 6 },
	{ 1 << 7, 1 << 4 },
	{ 1 << 4, 1 << 7 },
	// Face 4
	{ 1 << 6, 1 << 5 },
	{ 1 << 6, 1 << 4 },
	{ 1 << 4, 1 << 6 },
	{ 1 << 5, 1 << 6 },
	// Face 5
	{ 1 << 3, 1 << 0 },
	{ 1 << 2, 1 << 0 },
	{ 1 << 1, 1 << 3 },
	{ 1 << 0, 1 << 3 },
	{ 1 << 0, 1 << 2 },
	// Face 6
	{ 1 << 3, 1 << 2 },
	{ 1 << 2, 1 << 1 },
	{ 1 << 1, 1 << 0 },
	{ 1 << 2, 1 << 3 },
	{ 1 << 1, 1 << 2 },
	{ 1 << 0, 1 << 1 },

};

/// <summary>
/// Defines how to look up the LEDs of a face in the led array above!
/// </summary>
struct Face
{
	int LedsStartIndex;
	int LedCount;
};

/// <summary>
/// Represent the offsets into the LEDs array for all 6 faces
/// </summary>
Face Faces[] =
{
	{ 0, 1 },
	{ 1, 2 },
	{ 3, 3 },
	{ 6, 4 },
	{ 10, 5 },
	{ 15, 6 }
};

/// <summary>
/// Initializes the GPIO expander to controls the LEDs
/// </summary>
void GPIOLEDs::init()
{
	wire.beginTransmission(DEV_ADRESS);   // transmit to IO Expander
	wire.write(PTR_CONFIG_REG);           // configure register pointer
	wire.write(0xFF);                     // define all pins as input
	wire.endTransmission();               // stop transmitting

	wire.beginTransmission(DEV_ADRESS);   // transmit to device #39
	wire.write(PTR_OUTPUT_REG);           // configure register pointer
	wire.write(0x00);                     // define all pins at 0 output level
	wire.endTransmission();               // stop transmitting
}

/// <summary>
/// Set a single LED on, based on its face and index in face
/// </summary>
void GPIOLEDs::set(int face, int led)
{
	set(ledIndex(face, led));
}

/// <summary>
/// Set a single LED on, based on the overall led index in led
/// description array defined above!
/// </summary>
/// <param name="ledIndex"></param>
void GPIOLEDs::set(int ledIndex)
{

	LED& rled = Leds[ledIndex];
	byte dir = ~(rled.HighMask | rled.LowMask); // All inputs except pins i and j
	byte out = rled.HighMask; // Only i pin is high

	wire.beginTransmission(DEV_ADRESS);
	wire.write(PTR_CONFIG_REG);
	wire.write(0xFF);
	int error = wire.endTransmission();     // stop transmitting
	if (error != 0)
	{
		console.print("Error setting config: ");
		console.println(error);
	}

	wire.beginTransmission(DEV_ADRESS);		// transmit to device 0x70
	wire.write(PTR_OUTPUT_REG);				// send adress of output register
	wire.write(out);						// sends byte
	error = wire.endTransmission();         // stop transmitting
	if (error != 0)
	{
		console.print("Error setting output: ");
		console.println(error);
	}

	wire.beginTransmission(DEV_ADRESS);
	wire.write(PTR_CONFIG_REG);
	wire.write(dir);
	error = wire.endTransmission();         // stop transmitting
	if (error != 0)
	{
		console.print("Error setting config: ");
		console.println(error);
	}
}

/// <summary>
/// Clears all LEDs
/// </summary>
void GPIOLEDs::clear()
{
	wire.beginTransmission(DEV_ADRESS);
	wire.write(PTR_CONFIG_REG);
	wire.write(0xFF);
	int error = wire.endTransmission();    // stop transmitting
	if (error != 0)
	{
		console.print("Error setting config: ");
		console.println(error);
	}
}

/// <summary>
/// Helper method to convert face+offset into led index
/// </summary>
int GPIOLEDs::ledIndex(int face, int led)
{
	return Faces[face].LedsStartIndex + led;
}

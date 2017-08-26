// Uses the IOExpander
// 
// 

#include "Dice_LED.h"
#include "DiceWire.h"
#include "Dice_Debug.h"

#define DEV_ADRESS  0x70  // Address of the IO expander!

#define PTR_INPUT_REG   0  // read only
#define PTR_OUTPUT_REG  1  // rw
#define PTR_POL_REG     2  // rw
#define PTR_CONFIG_REG  3  // rw

DiceLEDs LEDs;

// Internal LED representation
struct LED
{
	byte HighMask;	// What pin needs to be high, as a mask
	byte LowMask;	// What pin needs to be low, as a mask
};

// Define high/low pins for each LED
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

struct Face
{
	int LedsStartIndex;
	int LedCount;
};

// Represent the offsets into the LEDs array!
Face Faces[] =
{
	{ 0, 1 },
	{ 1, 2 },
	{ 3, 3 },
	{ 6, 4 },
	{ 10, 5 },
	{ 15, 6 }
};

void DiceLEDs::init()
{
	DiceWire.beginTransmission(DEV_ADRESS);   // transmit to IO Expander
	DiceWire.write(PTR_CONFIG_REG);           // configure register pointer
	DiceWire.write(0xFF);                     // define all pins as input
	DiceWire.endTransmission();               // stop transmitting

	DiceWire.beginTransmission(DEV_ADRESS);   // transmit to device #39
	DiceWire.write(PTR_OUTPUT_REG);           // configure register pointer
	DiceWire.write(0x00);                     // define all pins at 0 output level
	DiceWire.endTransmission();               // stop transmitting
}

void DiceLEDs::set(int face, int led)
{
	set(ledIndex(face, led));
}

void DiceLEDs::set(int ledIndex)
{
	LED& rled = Leds[ledIndex];
	byte dir = ~(rled.HighMask | rled.LowMask); // All inputs except pins i and j
	byte out = rled.HighMask; // Only i pin is high

	DiceWire.beginTransmission(DEV_ADRESS);
	DiceWire.write(PTR_CONFIG_REG);
	DiceWire.write(0xFF);
	int error = DiceWire.endTransmission();     // stop transmitting
	if (error != 0)
	{
		diceDebug.print("Error setting config: ");
		diceDebug.println(error);
	}

	DiceWire.beginTransmission(DEV_ADRESS);		// transmit to device 0x70
	DiceWire.write(PTR_OUTPUT_REG);				// send adress of output register
	DiceWire.write(out);						// sends byte
	error = DiceWire.endTransmission();         // stop transmitting
	if (error != 0)
	{
		diceDebug.print("Error setting output: ");
		diceDebug.println(error);
	}

	DiceWire.beginTransmission(DEV_ADRESS);
	DiceWire.write(PTR_CONFIG_REG);
	DiceWire.write(dir);
	error = DiceWire.endTransmission();         // stop transmitting
	if (error != 0)
	{
		diceDebug.print("Error setting config: ");
		diceDebug.println(error);
	}

}

void DiceLEDs::clear()
{
	DiceWire.beginTransmission(DEV_ADRESS);
	DiceWire.write(PTR_CONFIG_REG);
	DiceWire.write(0xFF);
	int error = DiceWire.endTransmission();    // stop transmitting
	if (error != 0)
	{
		diceDebug.print("Error setting config: ");
		diceDebug.println(error);
	}
}

int DiceLEDs::ledIndex(int face, int led)
{
	return Faces[face].LedsStartIndex + led;
}

#include "APA102LEDs.h"
#include "I2C.h"
#include "Console.h"
#include "MessageQueue.h"
#include <Adafruit_DotStar.h>
#include <SPI.h>

using namespace Core;
using namespace Devices;
using namespace Systems;

// Here's how to control the LEDs from any two pins:
#define DATAPIN		7
#define CLOCKPIN	6
#define NUMPIXELS	21

// The actual led strip!
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

///// <summary>
///// Define led index in strip as per the routing of the pcb
///// </summary>
//int theLeds[] =
//{
//	// Face 1
//	17,
//	// Face 2
//	15, 16,
//	// Face 3
//	18, 19, 20,
//	// Face 4
//	2, 3, 4, 5,
//	// Face 5
//	0, 1, 6, 7, 8,
//	// Face 6
//	9, 10, 11, 12, 13, 14
//};
//
/// <summary>
/// Define led index in strip as per the routing of the pcb
/// </summary>
int theLeds[] =
{
	// Face 1
	17,
	// Face 2
	15, 16,
	// Face 3
	18, 19, 20,
	// Face 4
	2, 3, 0, 1,
	// Face 5
	4, 5, 7, 6, 8,
	// Face 6
	9, 12, 10, 11, 13, 14
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
Face RGBFaces[] =
{
	{ 0, 1 },
	{ 1, 2 },
	{ 3, 3 },
	{ 6, 4 },
	{ 10, 5 },
	{ 15, 6 }
};


void APA102LEDs::init()
{
	strip.begin(); // Initialize pins for output
	for (int i = 0; i < NUMPIXELS; ++i)
	{
		strip.setPixelColor(i, 0);
	}
	strip.show();  // Turn all LEDs off ASAP
}

void APA102LEDs::set(int face, int led, uint32_t color, bool flush)
{
	set(ledIndex(face, led), color, flush);
}

void APA102LEDs::set(int ledIndex, uint32_t color, bool flush)
{
	int theled = theLeds[ledIndex];
	//console.print("setting led ");
	//console.print(theled);

	strip.setPixelColor(theled, color);

	//console.print(" to ");
	//console.println(color, HEX);

	if (flush)
	{
		show();
	}
}

void APA102LEDs::show()
{
	strip.show();
}

void APA102LEDs::clear()
{
	for (int i = 0; i < NUMPIXELS; ++i)
	{
		strip.setPixelColor(i, 0);
	}
	strip.show();
}

int APA102LEDs::ledIndex(int face, int led)
{
	return RGBFaces[face].LedsStartIndex + led;
}


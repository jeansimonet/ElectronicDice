#include "LEDs.h"
#include "Die.h"
#include <SimbleeBLE.h>

Die die;

#include "Console.h"
#include "I2C.h"
#include "Accelerometer.h"


// Here's how to control the LEDs from any two pins:
#define DATAPIN		7
#define CLOCKPIN	0
#define NUMPIXELS	21


using namespace Systems;
using namespace Devices;


void setup() {
	//wire.begin();
	die.init();
	//strip.begin(); // Initialize pins for output
	//for (int i = 0; i < NUMPIXELS; ++i)
	//{
	//	strip.setPixelColor(i, 0x0F0000);
	//}
	//strip.show();  // Turn all LEDs off ASAP
}

void SimbleeBLE_onConnect()
{
	//die.onConnect();
}

void SimbleeBLE_onDisconnect()
{
	//die.onDisconnect();
}

void SimbleeBLE_onReceive(char *data, int len)
{
	//die.onReceive(data, len);
}

void loop()
{
	//for (int i = 0; i < NUMPIXELS; ++i)
	//{
	//	strip.setPixelColor(i, 0x0F0F00);
	//}
	//strip.show();  // Turn all LEDs off ASAP
				   
	die.update();

	//wire.beginTransmission(0x50);
	//wire.write(0x00);
	//wire.endTransmission();

}



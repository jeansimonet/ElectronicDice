#include "HWTesting.h"
#include "I2C.h"
#include "Accelerometer.h"
#include "Adafruit_DotStar.h"
#include "Accelerometer.h"

using namespace Devices;

/// <summary>
/// Writes to the serial port
/// </summary>
void Tests::TestSerial()
{
	Serial.begin(9600);
	Serial.println("Testing Serial port");
	while (true)
	{
		Serial.println("Testing");
		delay(500);
	}
}

/// <summary>
/// Repeatedly attempts to write to I2C devices, so you can check that the lines are toggling on and off!
/// </summary>
void Tests::TestI2C()
{
	Serial.begin(9600);
	Serial.println("Trying to Write to arbitrary I2C addresses...");
	Systems::wire.begin();
	pinMode(0, OUTPUT);
	while (true)
	{
		digitalWrite(0, HIGH);
		for (byte address = 1; address < 127; address++)
		{
			Systems::wire.beginTransmission(address);
			Systems::wire.write(0);
			Systems::wire.endTransmission();
		}
		digitalWrite(0, LOW);
	}
}

/// <summary>
/// Attempts to read from the Accelerometer repeatedly
/// </summary>
void Tests::TestAcc()
{
	Serial.begin(9600);
	Serial.println("Initializing I2C...");
	// Initialize I2C
	Systems::wire.begin();

	while (true)
	{
		Serial.println("Trying to read from Accelerometer...");
		Systems::wire.beginTransmission(0x1C);
		Systems::wire.write(WHO_AM_I);
		uint8_t ret = Systems::wire.endTransmission(false); //endTransmission but keep the connection active
		switch (ret)
		{
		case 4:
			Serial.println("Unknown error");
			break;
		case 3:
			Serial.println("NACK on Data");
			break;
		case 2:
			Serial.println("NACK on Address");
			break;
		case 1:
			Serial.println("Data too long");
			break;
		case 0:
			{
				Systems::wire.requestFrom((uint8_t)0x1C, (byte)1); // Ask for 1 byte, once done, bus is released by default

				int start = millis();
				bool timeout = false;
				while (!Systems::wire.available() && !timeout)
				{
					timeout = (millis() - start) > 1000;
				}

				if (timeout)
				{
					Serial.println("Timeout waiting for data");
				}
				else
				{
					byte c = Systems::wire.read(); //Return this one byte
					if (c != 0x2A) // WHO_AM_I should always be 0x2A
					{
						Serial.print("Wrong device id, got ");
						Serial.print(c, HEX);
						Serial.println(", expected 2A");
					}
					else
					{
						Serial.println("Ok");
						//Serial.println("Reading acceleration");

						//// Read registers!
						//for (int i = 0; i < 1000; ++i)
						//{
						//	byte rawData[6];  // x/y/z accel register data stored here
						//	Systems::wire.beginTransmission(0x1C);
						//	Systems::wire.write(OUT_X_MSB);
						//	Systems::wire.endTransmission(false); //endTransmission but keep the connection active

						//	Systems::wire.requestFrom(0x1C, 6); //Ask for bytes, once done, bus is released by default

						//	while (Systems::wire.available() < 6); //Hang out until we get the # of bytes we expect

						//	for (int x = 0; x < 6; x++)
						//		rawData[x] = Systems::wire.read();

						//	short x = ((short)(rawData[0] << 8 | rawData[1])) >> 4;
						//	short y = ((short)(rawData[2] << 8 | rawData[3])) >> 4;
						//	short z = ((short)(rawData[4] << 8 | rawData[5])) >> 4;
						//	Serial.print("x: ");
						//	Serial.print(x);
						//	Serial.print(", y: ");
						//	Serial.print(y);
						//	Serial.print(", z: ");
						//	Serial.println(z);
						//}
					}
				}
			}
			break;
		}
	}
}


extern Adafruit_DotStar strip;
#define NUMPIXELS	21
#define POWERPIN	4

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
	if (WheelPos < 85)
	{
		return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	}
	else if (WheelPos < 170)
	{
		WheelPos -= 85;
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	else
	{
		WheelPos -= 170;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
	uint16_t i, j;

	for (j = 0; j<256; j++)
	{
		for (i = 0; i< NUMPIXELS; i++)
		{
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

/// <summary>
/// Drive the LEDs Repeatedly
/// </summary>
void Tests::TestLED()
{
	Serial.begin(9600);
	Serial.println("Trying to Control APA102 LEDs.");

	strip.begin();
	while (true)
	{
		Serial.println("Forcing Power ON.");
		pinMode(POWERPIN, OUTPUT);
		digitalWrite(POWERPIN, 0);

		rainbowCycle(5);

		Serial.println("Forcing Power OFF.");
		pinMode(POWERPIN, OUTPUT);
		digitalWrite(POWERPIN, 1);

		rainbowCycle(5);

		Serial.println("Forcing Input");
		pinMode(POWERPIN, INPUT);

		rainbowCycle(5);
	}
}

/// <summary>
/// Drive the LEDs Repeatedly
/// </summary>
void Tests::TestLEDPower()
{
	Serial.begin(9600);
	Serial.println("Trying to Control APA102 LEDs Power pin.");
	pinMode(POWERPIN, OUTPUT);
	while (true)
	{
		pinMode(POWERPIN, OUTPUT);
		digitalWrite(POWERPIN, 0);
		delay(1000);
		pinMode(POWERPIN, OUTPUT);
		digitalWrite(POWERPIN, 1);
		delay(1000);
		pinMode(POWERPIN, INPUT);
		delay(1000);
	}
}

void Tests::TestSleepForever()
{
	Serial.begin(9600);
	Serial.println("Going to sleep forever now, bye!");

	// Sleep forever
	Simblee_ULPDelay(INFINITE);
}

#define radioPin 31
#define accelPin 23


void Tests::TestSleepAwakeAcc()
{
	Serial.begin(9600);

	Serial.print("Initializing accelerometer");
	accelerometer.init();

	while (true)
	{
		// Setup interrupt on accelerometer
		Serial.print("Setting up accelerometer, and ");
		accelerometer.enableTransientInterrupt();

		// Prepare to wakeup on matching interrupt pin
		Simblee_pinWake(accelPin, LOW);

		// Sleep forever
		Serial.println("going to sleep...");
		Simblee_ULPDelay(INFINITE);

		// If we get here, we either got an accelerometer interrupt, or bluetooth message

		// Reset both pinwake flags
		if (Simblee_pinWoke(accelPin))
			Simblee_resetPinWake(accelPin);

		// Disable accelerometer interrupts
		accelerometer.clearTransientInterrupt();
		accelerometer.disableTransientInterrupt();

		// Disable pinWake
		Simblee_pinWake(accelPin, DISABLE);

		Serial.println("...And I'm back!");
		delay(1000);
		Serial.print("3...");
		delay(1000);
		Serial.print("2...");
		delay(1000);
		Serial.print("1...");
		delay(1000);
	}
}

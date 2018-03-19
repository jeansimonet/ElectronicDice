#include "HWTesting.h"
#include "I2C.h"
#include "Accelerometer.h"
#include "Adafruit_DotStar.h"

/// <summary>
/// Repeatedly attempts to write to I2C devices, so you can check that the lines are toggling on and off!
/// </summary>
void Tests::TestI2C()
{
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
	// Initialize I2C
	Systems::wire.begin();

	while (true)
	{
		Serial.print("Trying to read from Accelerometer...");
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
					}
				}
			}
			break;
		}
	}
}


extern Adafruit_DotStar strip;
#define NUMPIXELS	21

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
	strip.begin();
	while (true)
	{
		Serial.println("Trying to Control APA102 LEDs.");
		rainbowCycle(5);
	}
}
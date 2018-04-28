#include "HWTesting.h"
#include "I2C.h"
#include "Accelerometer.h"
#include "Adafruit_DotStar.h"
#include "Accelerometer.h"
#include "Timer.h"
#include "LEDs.h"
#include "AnimController.h"
#include "Settings.h"
#include "AnimationSet.h"

using namespace Devices;
using namespace Systems;

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
					}
				}
			}
			break;
		}
	}
}


/// <summary>
/// Attempts to read from the Accelerometer repeatedly
/// </summary>
void Tests::TestAccDice()
{
	Serial.begin(9600);
	Serial.print("Initializing I2C...");
	Systems::wire.begin();
	Serial.println("Ok...");


	Serial.print("Initializing accelerometer");
	accelerometer.init();
	Serial.println("Ok");

	Serial.println("Trying to read from Accelerometer...");
	while (true)
	{
		accelerometer.read();
		Serial.print("x: ");
		Serial.print(accelerometer.cx);
		Serial.print("  y: ");
		Serial.print(accelerometer.cy);
		Serial.print("  z: ");
		Serial.println(accelerometer.cz);
		delay(500);
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
	pinMode(POWERPIN, OUTPUT);
	digitalWrite(POWERPIN, 0);

	strip.begin();
	while (true)
	{
		rainbowCycle(5);
	}
}

/// <summary>
/// Drive the LEDs Repeatedly
/// </summary>
void Tests::TestLEDSlow()
{
	Serial.begin(9600);
	Serial.println("Trying to Control APA102 LEDs.");
	pinMode(POWERPIN, OUTPUT);
	digitalWrite(POWERPIN, 0);

	strip.begin();
	while (true)
	{
		digitalWrite(POWERPIN, 0);
		rainbowCycle(5);
		for (int i = 0; i < 21; ++i)
			strip.setPixelColor(i, 0);
		strip.show();
		digitalWrite(POWERPIN, 1);
		delay(3000);
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

void Tests::TestLEDDice()
{
	Serial.begin(9600);
	Serial.print("Initializing LEDs...");
	leds.init();
	Serial.println("Ok");

	while (true)
	{
		Serial.println("Increasing brightness from black to white");
		for (int b = 0; b < 256; ++b)
		{
			leds.setAll(b | (b << 8) | (b << 16));
			delay(10);
		}
		for (int b = 255; b >= 0; --b)
		{
			leds.setAll(b | (b << 8) | (b << 16));
			delay(10);
		}
		Serial.println("Cycling colors");
		for (int k = 0; k < 5; ++k)
		{
			for (int j = 0; j<256; j++)
			{
				leds.setAll(Wheel(j));
				delay(5);
			}
		}
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
#define accelPin 20


void Tests::TestSleepAwakeAcc()
{
	Serial.begin(9600);
	Serial.print("Initializing I2C...");
	Systems::wire.begin();
	Serial.println("Ok");

	Serial.print("Initializing accelerometer...");
	accelerometer.init();
	Serial.println("Ok");

	// Set accelerometer interrupt pin as an input!
	pinMode(accelPin, INPUT_PULLUP);

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

void Tests::TestSettings()
{
	Serial.begin(9600);

	Serial.print("Checking Settings...");
	bool ok = settings->CheckValid();
	if (ok)
	{
		Serial.println("Ok");
		Serial.print("Dice name: ");
		Serial.println(settings->name);
	}
	else
	{
		Serial.println("Not initialized");
	}

	Serial.print("Erasing settings flash page");
	if (Settings::EraseSettings())
	{
		Serial.println("Ok");
		Serial.print("Writing some settings...");
		Settings settingsToWrite;
		strncpy(settingsToWrite.name, "TestingSettings", 16);
		if (Settings::TransferSettings(&settingsToWrite))
		{
			Serial.println("Ok");
			Serial.println("Settings content");
			Serial.println(settings->headMarker, HEX);
			Serial.println(settings->name);
			Serial.println(settings->tailMarker, HEX);

			Serial.print("Checking settings again...");
			ok = settings->CheckValid();
			if (ok)
			{
				Serial.println("Ok");
				Serial.print("Dice name: ");
				Serial.println(settings->name);
			}
			else
			{
				Serial.println("Not initialized");
			}
		}
		else
		{
			Serial.println("Error writing settings");
		}
	}
	else
	{
		Serial.println("Error erasing flash");
	}
}

void Tests::TestAnimationSet()
{
	auto printAnimationSet = []()
	{
		Serial.print("Set contains ");
		Serial.print(animationSet->Count());
		Serial.println(" animations");

		for (int i = 0; i < animationSet->Count(); ++i)
		{
			auto anim = animationSet->GetAnimation(i);
			Serial.print("Anim ");
			Serial.print(i);
			Serial.print(" contains ");
			Serial.print(anim->TrackCount());
			Serial.println(" tracks");

			for (int j = 0; j < anim->TrackCount(); ++j)
			{
				auto& track = anim->GetTrack(j);
				Serial.print("Anim ");
				Serial.print(i);
				Serial.print(", track ");
				Serial.print(j);
				Serial.print(" has ");
				Serial.print(track.count);
				Serial.print(" keyframes, starts at ");
				Serial.print(track.startTime);
				Serial.print(" ms, lasts ");
				Serial.print(track.duration);
				Serial.print(" ms and controls LED ");
				Serial.println(track.ledIndex);

				for (int k = 0; k < track.count; ++k)
				{
					auto& keyframe = track.keyframes[k];
					Serial.print("(");
					Serial.print(keyframe.time);
					Serial.print("ms = ");
					Serial.print(keyframe.red);
					Serial.print(", ");
					Serial.print(keyframe.green);
					Serial.print(", ");
					Serial.print(keyframe.blue);
					Serial.print(") ");
				}
			}
		}
		Serial.println();
	};

	Serial.begin(9600);

	Serial.print("Checking AnimationSet...");
	bool ok = animationSet->CheckValid();
	if (ok)
	{
		Serial.println("Ok");
		printAnimationSet();
	}
	else
	{
		Serial.println("Not initialized");
	}

	// We're going to program a few animations!
	// Create them
	AnimationTrack updown;
	updown.keyframes[0].time = 0;
	updown.keyframes[0].red = 0;
	updown.keyframes[0].green = 0;
	updown.keyframes[0].blue = 0;

	updown.keyframes[1].time = 128;
	updown.keyframes[1].red = 255;
	updown.keyframes[1].green = 255;
	updown.keyframes[1].blue = 255;

	updown.keyframes[2].time = 255;
	updown.keyframes[2].red = 0;
	updown.keyframes[2].green = 0;
	updown.keyframes[2].blue = 0;

	updown.count = 3;
	updown.startTime = 0;	// ms
	updown.duration = 1000;	// ms
	updown.ledIndex = 0;
	
	Animation* anim1 = Animation::AllocateAnimation(1);
	anim1->SetTrack(updown, 0);

	Serial.print("Erasing animation flash pages...");
	AnimationSet::ProgrammingToken token;
	if (AnimationSet::EraseAnimations(anim1->ComputeByteSize(), token))
	{
		Serial.println("Ok");
		Serial.print("Writing 1 animation...");
		if (AnimationSet::TransferAnimation(anim1, token))
		{
			Serial.println("Ok");
			Serial.print("Writing animation set...");
			if (AnimationSet::TransferAnimationSet(token.animationPtrInFlash, token.currentCount))
			{
				Serial.println("Ok");

				// Clean up memory
				free(anim1);

				Serial.print("Checking AnimationSet again...");
				if (animationSet->CheckValid())
				{
					Serial.println("Ok");
					printAnimationSet();
				}
				else
				{
					Serial.println("Not initialized");
				}
			}
			else
			{
				Serial.println("Error writing animation set");
			}
		}
		else
		{
			Serial.println("Error writing animation");
		}
	}
	else
	{
		Serial.println("Error erasing flash");
	}
}


void Tests::TestTimerSetup()
{
	Serial.begin(9600);

	Serial.print("Setting some callback...");
	Systems::timer.hook(100000, [](void* ignore) {Serial.println("Callback!"); }, nullptr);
	Serial.println("Ok");

	Serial.print("Initializing Timer...");
	Systems::timer.begin();
	Serial.println("Ok");
}

void Tests::TestTimerUpdate()
{
	Systems::timer.update();
}

void Tests::TestAnimationsSetup()
{
	Serial.begin(9600);

	Serial.print("Initializing LEDs...");
	Serial.println("Ok");


	Serial.print("Initializing animation controller...");
	animController.begin();
	Serial.println("Ok");

	Serial.print("Initializing Timer...");
	Systems::timer.begin();
	Serial.println("Ok");
}

void Tests::TestAnimationsUpdate()
{

}

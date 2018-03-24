#include <SimbleeBLE.h>

#include "Die.h"
#include "Timer.h"
#include "I2C.h"
#include "Debug.h"
#include "Lazarus.h"

#include "Accelerometer.h"
#include "LEDs.h"

#include "BluetoothMessage.h"
#include "CommandMessage.h"
#include "AccelController.h"
#include "Settings.h"
#include "AnimController.h"

#include "LEDAnimations.h"
#include "Utils.h"
#include "Telemetry.h"
#include "JerkMonitor.h"

#include "EstimatorOnFace.h"

using namespace Core;
using namespace Systems;
using namespace Devices;

Die::Die()
{
	currentFace = 0;
}

EstimatorOnFace estimatorOnFace;

void Die::init()
{
#if defined(_CONSOLE)
	console.begin();
#endif
	// put your setup code here, to run once:
	//setup I2C on the pins of your choice
	debugPrint("LED init...");
	leds.init();
	debugPrintln("ok");

	// Flash all the LEDs once, to make sure they work!
	leds.setAll(0x000F04);
	delay(300);
	leds.clearAll();

	// Then turn an led on the 6th face after major init code
	leds.setLED(5, 0, 0xFFFF00);
	debugPrint("Wire init...");
	wire.begin();
	debugPrintln("ok");
	//leds.init(); // Depends on I2C
	leds.setLED(5, 0, 0x00FF00);

	debugPrint("Accelerometer init...");
	leds.setLED(5, 1, 0xFFFF00);
	accelerometer.init();
	debugPrintln("ok");
	leds.setLED(5, 1, 0x00FF00);

	leds.setLED(5, 2, 0xFFFF00);
	debugPrint("Settings init...");
	settings.init();
	debugPrintln("ok");
	leds.setLED(5, 2, 0x00FF00);

	leds.setLED(5, 3, 0xFFFF00);
	debugPrint("BLE init...");
	// start the BLE stack
	SimbleeBLE.end();
	SimbleeBLE.advertisementData = settings.name;
	SimbleeBLE.deviceName = settings.name;
	SimbleeBLE.txPowerLevel = 4;
	SimbleeBLE.begin();
	debugPrintln("ok");
	leds.setLED(5, 3, 0x00FF00);

	leds.setLED(5, 4, 0xFFFF00);
	debugPrint("Modules init...");
	animController.begin(); // Talks to controller
	accelController.begin();
	telemetry.begin();
	lazarus.init();
	jerkMonitor.begin();
	debugPrintln("ok");
	leds.setLED(5, 4, 0x00FF00);

	leds.setLED(5, 5, 0xFFFF00);
	//estimatorOnFace.begin();

	//stateEstimators[DieState_OnFace] = &estimatorOnFace;

	leds.setLED(5, 5, 0x00FF00);
	delay(300);
	leds.clearAll();

	timer.begin(); // Kicks off the timers!
}

void Die::onConnect()
{
	// Wake up if necessary!
	lazarus.onRadio();
	
	// Insert code
	debugPrintln("Connected!");
}

void Die::onDisconnect()
{
	// Insert code here
	debugPrintln("Disconnected!");
}

void Die::onReceive(char *data, int len)
{
	// Wake up if necessary
	lazarus.onRadio();

	if (len >= sizeof(CommandMessage))
	{
		auto msg = reinterpret_cast<CommandMessage*>(data);
		processCommand(msg);
	}
}

void Die::update()
{
	// Update systems that need it!
#if defined(_CONSOLE)
	processConsole();
#endif

	timer.update();
	updateFaceAnimation();
	//lazarus.update();

	//// Ask the estimators which state we should be in...
	//for (int i = 0; i < DieState_Count; ++i)
	//{
	//	currentState.estimates[i] = stateEstimators[i]->GetEstimate();
	//}
}

void Die::processCommand(CommandMessage* msg)
{
	switch (msg->type)
	{
	case CommandMessage::CommandType_PlayAnim:
		{
			auto animMsg = static_cast<CommandMessagePlayAnim*>(msg);
			playAnimation(animMsg->animation);
		}
		break;
	default:
		break;
	}
}

void Die::updateFaceAnimation()
{
	int newFace = accelController.currentFace();
	if (newFace != currentFace)
	{
		if (!SimbleeBLE_radioActive)
		{
			currentFace = newFace;

			// Toggle leds
			animController.stopAll();
			playAnimation(currentFace);

			debugPrint("sending face number ");
			debugPrintln(currentFace);

			// Send face message
			DieMessageFace faceMessage;
			faceMessage.type = DieMessage::MessageType_Face;
			faceMessage.face = currentFace;
			SimbleeBLE.send(reinterpret_cast<const char*>(&faceMessage), sizeof(DieMessageFace));
		}
	}
}

#if defined(_CONSOLE)
/// <summary>
/// Processes any input on the serial port, if any
/// </summary>
void Die::processConsole()
{
	if (console.available() > 0)
	{
		char buffer[32];
		int len = console.readBytesUntil('\n', buffer, 32);
		processConsoleCommand(buffer, len);
	}
}
#endif

#if defined(_CONSOLE)
const char Keyword_PlayAnim[] = "playanim";
const char Keyword_SetName[] = "name";
const char Keyword_ClearLEDs[] = "clearleds";
const char Keyword_SetLED[] = "setled";

const char Keyword_Help[] = "help";

/// /// <summary>
/// Processes a console command, parsing the command name and parameters
/// </summary>
/// <param name="data">The command buffer</param>
/// <param name="len">The length of the command buffer</param>
void Die::processConsoleCommand(char* data, int len)
{
	char commandWord[20] = "";
	int commandLength = parseWord(data, len, commandWord, 20);
	if (commandLength > 0)
	{
		if (strcmp(commandWord, Keyword_PlayAnim) == 0)
		{
			// Parse animation number
			char number[4] = "";
			int numberLength = parseWord(data, len, number, 4);
			if (numberLength > 0)
			{
				// Convert the number and run the animation!
				int animNumber = atoi(number);
				playAnimation(animNumber);
			}
		}
		else if (strcmp(commandWord, Keyword_SetName) == 0)
		{
			// Parse name, or display it!
			char newName[17] = "";
			int nameLength = parseWord(data, len, newName, 17);
			if (nameLength > 0)
			{
				// Set the new name
				strcpy(settings.name, newName);
				settings.writeSettings();
			}
			// In either case, print out the name
			debugPrint("Name is: ");
			debugPrintln(settings.name);
		}
		else if (strcmp(commandWord, Keyword_ClearLEDs) == 0)
		{
			leds.clearAll();
		}
		else if (strcmp(commandWord, Keyword_SetLED) == 0)
		{
			// Parse number of face
			char face[4] = "";
			int faceLength = parseWord(data, len, face, 4);
			if (faceLength > 0)
			{
				char led[4] = "";
				int ledLength = parseWord(data, len, led, 4);
				if (ledLength > 0)
				{
					char color[10] = "";
					int colorLength = parseWord(data, len, color, 10);
					if (colorLength)
					{
						int f = atoi(face);
						int l = atoi(led);
						uint32_t c = strtol(color, nullptr, 16);
						leds.setLED(f, l, c);
					}
				}
			}
		}
		else if (strcmp(commandWord, Keyword_Help) == 0)
		{
			debugPrintln("Possible commands:");
			debugPrintln("  playanim <number> - Plays one of the face animations");
			debugPrintln("  name <name, optional> - display (or sets) the bluetooth name of the die");
			debugPrintln("  clearleds - Turns all LEDs off");
			debugPrintln("  setled <face> <index> <color> - Sets the given LED to the passed in color");
		}
		else
		{
			debugPrint("Unknown command \'");
			debugPrint(commandWord);
			debugPrintln("\', type help for list of available commands");
		}
	}
}
#endif

void Die::playAnimation(int animIndex)
{
	switch (animIndex)
	{
	case 0:
		animController.play(&LEDAnimations::FaceOneSlowPulse);
		break;
	case 1:
		animController.play(&LEDAnimations::rotatingTwo);
		break;
	case 2:
		animController.play(&LEDAnimations::rotatingThree);
		break;
	case 3:
		animController.play(&LEDAnimations::rotatingFour);
		break;
	case 4:
		animController.play(&LEDAnimations::FaceFiveCross);
		break;
	case 5:
		animController.play(&LEDAnimations::rotatingSix);
		break;
	case 6:
		animController.play(&LEDAnimations::randomLEDs);
	}
}

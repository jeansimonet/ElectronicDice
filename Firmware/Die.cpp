#include <SimbleeBLE.h>

#include "Die.h"
#include "Timer.h"
#include "I2C.h"
#include "Console.h"
#include "MessageQueue.h"
#include "Lazarus.h"

#include "Accelerometer.h"
#include "LEDs.h"
#include "EEPROM.h"

#include "BluetoothMessage.h"
#include "CommandMessage.h"
#include "AccelController.h"
#include "Settings.h"
#include "AnimController.h"

#include "LEDAnimations.h"
#include "MessageQueue.h"
#include "Utils.h"

using namespace Core;
using namespace Systems;
using namespace Devices;


const char Keyword_PlayAnim[] = "playanim";
const char Keyword_SetName[] = "name";
const char Keyword_DumpEEPROM[] = "eedump";
const char Keyword_ClearLEDs[] = "clearleds";
const char Keyword_SetLED[] = "setled";

const char Keyword_Help[] = "help";

Die::Die()
	: accelController(messageQueue)
	, animController()
	, settings()
{
	currentFace = 0;
}

void Die::init()
{
	// put your setup code here, to run once:
	//setup I2C on the pins of your choice
	console.begin();
	console.print("LED init...");
	leds.init();
	console.println("ok");

	// Flash all the LEDs once, to make sure they work!
	leds.setAllNow(0x000F04);
	delay(300);
	leds.clearAllNow();

	// Then turn an led on the 6th face after major init code
	leds.setLEDNow(5, 0, 0xFFFF00);
	console.print("Wire init...");
	wire.begin();
	console.println("ok");
	//leds.init(); // Depends on I2C
	leds.setLEDNow(5, 0, 0x00FF00);

	console.print("Accelerometer init...");
	leds.setLEDNow(5, 1, 0xFFFF00);
	accelerometer.init();
	console.println("ok");
	leds.setLEDNow(5, 1, 0x00FF00);

	//leds.setLEDNow(5, 2, 0xFFFF00);
	//console.print("Settings init...");
	//settings.init();
	//console.println("ok");
	//leds.setLEDNow(5, 2, 0x00FF00);

	leds.setLED(5, 3, 0xFFFF00);
	console.print("BLE init...");

	// start the BLE stack
	// put your setup code here, to run once:
	SimbleeBLE.advertisementData = settings.name;
	SimbleeBLE.deviceName = settings.name;
	SimbleeBLE.txPowerLevel = 4;
	SimbleeBLE.begin();

	console.println("ok");
	leds.setLEDNow(5, 3, 0x00FF00);

	leds.setLEDNow(5, 4, 0xFFFF00);
	console.print("Modules init...");
	animController.begin(); // Talks to controller
	accelController.begin();
	messageQueue.init();
	console.println("ok");
	leds.setLEDNow(5, 4, 0x00FF00);

	leds.setLEDNow(5, 5, 0xFFFF00);

	timer.begin(); // Kicks off the timers!

	// Init our reuseable telemetry message
	teleMessage.type = DieMessage::MessageType_Telemetry;
	for (int i = 0; i < 1; ++i)
	{
		teleMessage.data[i] = { 0,0,0 };
	}

	leds.setLEDNow(5, 5, 0x00FF00);
	delay(300);
	leds.clearAllNow();
}

void Die::onConnect()
{
	// Wake up if necessary!
	lazarus.onRadio();
	
	// Insert code
	console.println("Connected!");
}

void Die::onDisconnect()
{
	// Insert code here
	console.println("Disconnected!");
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
	processConsole();
	processMessageQueue();
	updateFaceAnimation();
	leds.update();

	AccelFrame frame0;
	AccelFrame frame1;
	if (accelController.frameCount() >= 2 &&
		accelController.tryPop(frame0) &&
		accelController.tryPop(frame1))
	{
		// Build the bluetooth telemetry message
		teleMessage.data[0].x = frame0.X;
		teleMessage.data[0].y = frame0.Y;
		teleMessage.data[0].z = frame0.Z;
		teleMessage.data[0].deltaTime = frame0.Time - lastAccelFrame.Time;

		teleMessage.data[1].x = frame1.X;
		teleMessage.data[1].y = frame1.Y;
		teleMessage.data[1].z = frame1.Z;
		teleMessage.data[1].deltaTime = frame1.Time - frame0.Time;

		SimbleeBLE.send(reinterpret_cast<const char*>(&teleMessage), sizeof(DieMessageAcc));

		// Update tlast frame 
		lastAccelFrame = frame1;
	}
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

void Die::processMessageQueue()
{
	/// <summary>
	/// Updates the message queue, must somehow be called form main loop!
	/// </summary>
	MessageQueue::Message mes;
	while (messageQueue.tryDequeue(mes))
	{
		switch (mes.type)
		{
		case MessageType_UpdateAccel:
			accelerometer.read();
			break;
		case MessageType_UpdateFace:
			accelController.updateCurrentFace();
			break;
		default:
			break;
		}
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

			console.print("sending face number ");
			console.println(currentFace);

			// Send face message
			DieMessageFace faceMessage;
			faceMessage.type = DieMessage::MessageType_Face;
			faceMessage.face = currentFace;
			SimbleeBLE.send(reinterpret_cast<const char*>(&faceMessage), sizeof(DieMessageFace));
		}
	}
}

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

/// <summary>
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
			console.print("Name is: ");
			console.println(settings.name);
		}
		else if (strcmp(commandWord, Keyword_DumpEEPROM) == 0)
		{
			eeprom.dumpToConsole();
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
			console.println("Possible commands:");
			console.println("  playanim <number> - Plays one of the face animations");
			console.println("  name <name, optional> - display (or sets) the bluetooth name of the die");
			console.println("  eedump - dumps the content of the EEPROM to the console");
			console.println("  clearleds - Turns all LEDs off");
			console.println("  setled <face> <index> <color> - Sets the given LED to the passed in color");
		}
		else
		{
			console.print("Unknown command \'");
			console.print(commandWord);
			console.println("\', type help for list of available commands");
		}
	}
}

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
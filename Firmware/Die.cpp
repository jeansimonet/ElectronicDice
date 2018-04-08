#include <SimbleeBLE.h>

#include "Die.h"
#include "Timer.h"
#include "I2C.h"
#include "Debug.h"
#include "Lazarus.h"

#include "Accelerometer.h"
#include "LEDs.h"

#include "BluetoothMessage.h"
#include "AccelController.h"
#include "Settings.h"
#include "AnimController.h"
#include "Animation.h"
#include "AnimationSet.h"

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
	memset(messageHandlers, 0, sizeof(Die::HandlerAndToken) * DieMessage::MessageType_Count);
	updateHandlerCount = 0;
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
	debugPrint("Checking Settings...");
	if (settings->CheckValid())
	{
		debugPrintln("ok");
	}
	else
	{
		debugPrintln("invalid");
	}
	leds.setLED(5, 2, 0x00FF00);

	leds.setLED(5, 3, 0xFFFF00);
	debugPrint("BLE init...");

	// Hook default message handlers
	RegisterMessageHandler(DieMessage::MessageType_PlayAnim, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnPlayAnim(msg); });
	RegisterMessageHandler(DieMessage::MessageType_RequestAnimSet, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnRequestAnimSet(msg); });
	RegisterMessageHandler(DieMessage::MessageType_TransferAnimSet, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnUpdateAnimSet(msg); });

	// start the BLE stack
	SimbleeBLE.end();
	if (settings->CheckValid())
	{
		SimbleeBLE.advertisementData = settings->name;
		SimbleeBLE.deviceName = settings->name;
	}
	else
	{
		SimbleeBLE.advertisementData = "ElectronicDie";
		SimbleeBLE.deviceName = "ElectronicDie";
	}
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

	if (len >= sizeof(DieMessage))
	{
		auto msg = reinterpret_cast<DieMessage*>(data);
		auto handler = messageHandlers[(int)msg->type];
		if (handler.handler != nullptr)
		{
			handler.handler(handler.token, msg);
		}
	}
}

bool Die::SendMessage(DieMessage::MessageType msgType)
{
	DieMessage msg(msgType);
	return SimbleeBLE.send(reinterpret_cast<const char*>(&msg), sizeof(msg));
}

bool Die::SendMessage(const DieMessage* msg, int msgSize)
{
	return SimbleeBLE.send(reinterpret_cast<const char*>(msg), msgSize);
}


void Die::update()
{
	// Update systems that need it!
#if defined(_CONSOLE)
	processConsole();
#endif

	timer.update();
	updateFaceAnimation();
	lazarus.update();

	// Update handlers
	for (int i = 0; i < updateHandlerCount; ++i)
	{
		updateHandlers[i].handler(updateHandlers[i].token);
	}

	//// Ask the estimators which state we should be in...
	//for (int i = 0; i < DieState_Count; ++i)
	//{
	//	currentState.estimates[i] = stateEstimators[i]->GetEstimate();
	//}
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
			DieMessageState faceMessage;
			faceMessage.state = currentFace;
			SimbleeBLE.send(reinterpret_cast<const char*>(&faceMessage), sizeof(DieMessageState));
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

void Die::RegisterMessageHandler(DieMessage::MessageType msgType, void* token, DieMessageHandler handler)
{
	if (messageHandlers[msgType].handler != nullptr)
	{
		debugPrint("Handler for message ");
		debugPrint(msgType);
		debugPrintln(" already set");
	}
	else
	{
		messageHandlers[msgType].handler = handler;
		messageHandlers[msgType].token = token;
	}
}

void Die::UnregisterMessageHandler(DieMessage::MessageType msgType)
{
	messageHandlers[msgType].handler = nullptr;
	messageHandlers[msgType].token = nullptr;
}

void Die::RegisterUpdate(void* token, DieUpdateHandler handler)
{
	if (updateHandlerCount < UPDATE_MAX_COUNT)
	{
		updateHandlers[updateHandlerCount].handler = handler;
		updateHandlers[updateHandlerCount].token = token;
		updateHandlerCount++;
	}
	else
	{
		debugPrint("Too many update handlers");
	}
}

void Die::UnregisterUpdateHandler(DieUpdateHandler handler)
{
	int clientIndex = 0;
	for (; clientIndex < 4; ++clientIndex)
	{
		if (updateHandlers[clientIndex].handler == handler)
			break;
	}

	if (clientIndex != UPDATE_MAX_COUNT)
	{
		// Shift entries down
		updateHandlerCount--;
		for (; clientIndex < updateHandlerCount; ++clientIndex)
			updateHandlers[clientIndex] = updateHandlers[clientIndex + 1];
	}
}

void Die::UnregisterUpdateToken(void* token)
{
	int clientIndex = 0;
	for (; clientIndex < 4; ++clientIndex)
	{
		if (updateHandlers[clientIndex].token == token)
			break;
	}

	if (clientIndex != UPDATE_MAX_COUNT)
	{
		// Shift entries down
		updateHandlerCount--;
		for (; clientIndex < updateHandlerCount; ++clientIndex)
			updateHandlers[clientIndex] = updateHandlers[clientIndex + 1];
	}
}


void Die::OnPlayAnim(DieMessage* msg)
{
	auto playAnimMsg = static_cast<DieMessagePlayAnim*>(msg);
	playAnimation(playAnimMsg->animation);
}

void Die::OnRequestAnimSet(DieMessage* msg)
{
	// This will setup the data transfers and unregister itself when done
	sendAnimSetSM.Setup();
}

void Die::OnUpdateAnimSet(DieMessage* msg)
{
	auto updateAnimSetMsg = (DieMessageTransferAnimSet*)msg;
	// This will setup the data transfers and unregister itself when done
	receiveAnimSetSM.Setup(updateAnimSetMsg->count, updateAnimSetMsg->totalAnimationByteSize);
}


void Die::playAnimation(int animIndex)
{
	if (animationSet->CheckValid())
	{
		animController.play(animationSet->GetAnimation(animIndex));
	}
}


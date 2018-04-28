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
		debugPrintln("ok");
	else
		debugPrintln("invalid");
	leds.setLED(5, 2, 0x00FF00);

	leds.setLED(5, 3, 0xFFFF00);
	debugPrint("BLE init...");

	// Hook default message handlers
	RegisterMessageHandler(DieMessage::MessageType_PlayAnim, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnPlayAnim(msg); });
	RegisterMessageHandler(DieMessage::MessageType_RequestState, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnRequestState(msg); });
	RegisterMessageHandler(DieMessage::MessageType_RequestAnimSet, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnRequestAnimSet(msg); });
	RegisterMessageHandler(DieMessage::MessageType_TransferAnimSet, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnUpdateAnimSet(msg); });
	RegisterMessageHandler(DieMessage::MessageType_RequestSettings, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnRequestSettings(msg); });
	RegisterMessageHandler(DieMessage::MessageType_TransferSettings, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnUpdateSettings(msg); });
	RegisterMessageHandler(DieMessage::MessageType_RequestTelemetry, this, [](void* tok, DieMessage* msg) {((Die*)tok)->OnRequestTelemetry(msg); });

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

	updateFaceAnimation();

	// Update handlers
	for (int i = 0; i < updateHandlers.Count(); ++i)
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

void Die::PauseModules()
{
	timer.stop();
	lazarus.stop();
	leds.clearAll();
}

void Die::ResumeModules()
{
	lazarus.init();
	timer.begin();
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
		console.processCommand(buffer, len);
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
	if (!updateHandlers.Register(token, handler))
	{
		debugPrint("Too many update handlers");
	}
}

void Die::UnregisterUpdateHandler(DieUpdateHandler handler)
{
	updateHandlers.UnregisterWithHandler(handler);
}

void Die::UnregisterUpdateToken(void* token)
{
	updateHandlers.UnregisterWithToken(token);
}

void Die::OnPlayAnim(DieMessage* msg)
{
	auto playAnimMsg = static_cast<DieMessagePlayAnim*>(msg);
	playAnimation(playAnimMsg->animation);
}

void Die::OnRequestState(DieMessage* msg)
{
	// Send face message
	DieMessageState faceMessage;
	faceMessage.state = currentFace;
	SimbleeBLE.send(reinterpret_cast<const char*>(&faceMessage), sizeof(DieMessageState));
}

void Die::OnRequestAnimSet(DieMessage* msg)
{
	PauseModules();

	// This will setup the data transfers and unregister itself when done
	sendAnimSetSM.Setup(this, [](void* tok) {((Die*)tok)->ResumeModules(); });
}

void Die::OnUpdateAnimSet(DieMessage* msg)
{
	PauseModules();

	auto updateAnimSetMsg = (DieMessageTransferAnimSet*)msg;
	// This will setup the data transfers and unregister itself when done
	receiveAnimSetSM.Setup(updateAnimSetMsg->count, updateAnimSetMsg->totalAnimationByteSize,
		this, [](void* tok) {((Die*)tok)->ResumeModules(); });
}

void Die::OnRequestSettings(DieMessage* msg)
{
	PauseModules();

	// This will setup the data transfers and unregister itself when done
	sendSettingsSM.Setup(this, [](void* tok) {((Die*)tok)->ResumeModules(); });
}

void Die::OnUpdateSettings(DieMessage* msg)
{
	PauseModules();

	// This will setup the data transfers and unregister itself when done
	receiveSettingsSM.Setup(this, [](void* tok) {((Die*)tok)->ResumeModules(); });
}

void Die::OnRequestTelemetry(DieMessage* msg)
{
	auto telemMsg = (DieMessageRequestTelemetry*)msg;
	if (telemMsg->telemetry)
	{
		// Turn telemetry on
		telemetry.begin();
	}
	else
	{
		// Turn telemetry off
		telemetry.stop();
	}
}

void Die::playAnimation(int animIndex)
{
	if (animationSet->CheckValid())
	{
		animController.play(animationSet->GetAnimation(animIndex));
	}
}


// Die.h

#ifndef _DIE_h
#define _DIE_h

#include "Arduino.h"
#include "BluetoothMessage.h"
#include "MessageQueue.h"
#include "AccelController.h"
#include "AnimController.h"
#include "Settings.h"

// Forwards
class CommandMessage;

/// <summary>
/// This defines our global die object!
/// It includes all the systems and devices we need to work with
/// </summary>
class Die
{
public:

private:
	Core::MessageQueue messageQueue;

	// Modules
	AccelerationController accelController;
	AnimController animController;
	Settings settings;

	DieMessageAcc teleMessage;
	AccelFrame lastAccelFrame;
	int currentFace;

public:
	Die();
	void init();
	void onConnect();
	void onDisconnect();
	void onReceive(char *data, int len);
	void update();

private:
	void processCommand(CommandMessage* msg);
	void processMessageQueue();
	void updateFaceAnimation();
	void processConsole();
	void processConsoleCommand(char* data, int len);
	void playAnimation(int animIndex);
};

// The global die!
extern Die die;

#endif


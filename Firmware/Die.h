// Die.h

#ifndef _DIE_h
#define _DIE_h

#include "Arduino.h"
#include "BluetoothMessage.h"
#include "AccelController.h"
#include "AnimController.h"
#include "Settings.h"
#include "IStateEstimator.h"

// Forwards
class CommandMessage;

/// <summary>
/// This defines our global die object!
/// It coordinates all the systems and devices we need to work with
/// </summary>
class Die
{
private:
	int currentFace;
	StateEstimate currentState;
	IStateEstimator* stateEstimators[DieState_Count];

public:
	Die();
	void init();
	void onConnect();
	void onDisconnect();
	void onReceive(char *data, int len);
	void update();

private:
	void processCommand(CommandMessage* msg);
	void updateFaceAnimation();

#if defined(_CONSOLE)
	void processConsole();
	void processConsoleCommand(char* data, int len);
#endif
	void playAnimation(int animIndex);
};

// The global die!
extern Die die;

#endif


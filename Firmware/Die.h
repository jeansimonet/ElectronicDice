// Die.h

#ifndef _DIE_h
#define _DIE_h

#include "Arduino.h"
#include "BluetoothMessage.h"
#include "AccelController.h"
#include "AnimController.h"
#include "Settings.h"
#include "IStateEstimator.h"
#include "AnimationSet.h"

#define UPDATE_MAX_COUNT 4

class Animation;

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

	// Our bluetooth message handlers
	typedef void (*DieMessageHandler)(void* token, DieMessage* message);

	struct HandlerAndToken
	{
		DieMessageHandler handler;
		void* token;
	};
	HandlerAndToken messageHandlers[DieMessage::MessageType_Count];


	// Update message handlers
	typedef void(*DieUpdateHandler)(void* token);

	struct UpdateAndToken
	{
		DieUpdateHandler handler;
		void* token;
	};
	UpdateAndToken updateHandlers[UPDATE_MAX_COUNT];
	int updateHandlerCount;

	SendAnimSetSM sendAnimSetSM;
	ReceiveAnimSetSM receiveAnimSetSM;

public:
	Die();
	void init();
	void onConnect();
	void onDisconnect();
	void onReceive(char *data, int len);
	bool SendMessage(DieMessage::MessageType msgType);
	bool SendMessage(const DieMessage* msg, int msgSize);
	void update();

	void RegisterMessageHandler(DieMessage::MessageType msgType, void* token, DieMessageHandler handler);
	void UnregisterMessageHandler(DieMessage::MessageType msgType);

	void RegisterUpdate(void* token, DieUpdateHandler handler);
	void UnregisterUpdateHandler(DieUpdateHandler handler);
	void UnregisterUpdateToken(void* token);

private:
	void updateFaceAnimation();

#if defined(_CONSOLE)
	void processConsole();
	void processConsoleCommand(char* data, int len);
#endif
	void playAnimation(int animIndex);

	// Message handlers
	void OnPlayAnim(DieMessage* msg);
	void OnRequestAnimSet(DieMessage* msg);
	void OnUpdateAnimSet(DieMessage* msg);
};

// The global die!
extern Die die;

#endif


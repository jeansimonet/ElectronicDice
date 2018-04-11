// AnimationSet.h

#ifndef _ANIMATIONSET_h
#define _ANIMATIONSET_h

#include "arduino.h"
#include "Animation.h"
#include "BulkDataTransfer.h"

#define MAX_ANIMATIONS (64)
#define ANIMATION_SET_START_PAGE (250)
#define ANIMATION_SET_VALID_KEY (0x600DF00D) // Good Food ;)
// We place animation set and animations in descending addresses
// So the animation set is at the top of the page
#define ANIMATION_SET_ADDRESS ((ANIMATION_SET_START_PAGE + 1) * 1024 - sizeof(AnimationSet))

/// <summary>
/// The Animation set is the set of all die animations. It is mapped directly to flash!
/// </summary>
class AnimationSet
{
private:
	// Indicates whether there is valid data
	uint32_t headMarker;
	Animation* animations[MAX_ANIMATIONS];
	uint32_t count;
	uint32_t tailMarker;

public:
	bool CheckValid() const;
	int ComputeAnimationTotalSize() const;
	uint32_t Count() const;
	const Animation* GetAnimation(int index) const;
};

// The animation set in flash memory
extern const AnimationSet* animationSet;

/// <summary>
/// This defines a state machine that can manage receiving a new anim set
/// over bluetooth from the phone and then burn that animation set in flash.
/// </summary>
class ReceiveAnimSetSM
{
private:
	enum State
	{
		State_ErasingFlash = 0,
		State_SendingAck,
		State_TransferAnim,
		State_SendingReadyForNextAnim,
		State_Done
	};

	short count;
	short currentAnim;
	State currentState;
	uint32_t currentAnimFlashAddress;

	// Temporarily stores animation pointers as we program them in flash
	Animation** animationPtrInFlash;
	ReceiveBulkDataSM receiveBulkDataSM;

	typedef void(*FinishedCallback)(void* token);
	FinishedCallback FinishedCallbackHandler;
	void* FinishedCallbackToken;

private:
	void Finish();

public:
	ReceiveAnimSetSM();
	void Setup(short animCount, short totalAnimByteSize, void* token, FinishedCallback handler);
	void Update();
};

/// <summary>
/// This defines a state machine that can send the current animation set over
/// bluetooth to the phone. Typically so the phone can edit it and redownload it.
/// </summary>
class SendAnimSetSM
{
private:
	enum State
	{
		State_SendingSetup,
		State_WaitingForSetupAck,
		State_SetupAckReceived,
		State_SendingAnim,
		State_WaitingForReadyForNextAnim,
		State_ReceivedReadyForNextAnim,
		State_Done
	};

	short currentAnim;
	State currentState;

	// Temporarily stores animation pointers as we program them in flash
	SendBulkDataSM sendBulkDataSM;

	typedef void(*FinishedCallback)(void* token);
	FinishedCallback FinishedCallbackHandler;
	void* FinishedCallbackToken;

private:
	void Finish();

public:
	SendAnimSetSM();
	void Setup(void* token, FinishedCallback handler);
	void Update();
};


#endif


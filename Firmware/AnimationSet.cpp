#include "AnimationSet.h"
#include "Debug.h"
#include "Die.h"
#include "SimbleeBLE.h"
#include "BluetoothMessage.h"

// The animation set always points at a specific address in memory
const AnimationSet* animationSet = (const AnimationSet*)ANIMATION_SET_ADDRESS;

/// <summary>
/// Checks whether the animation set in flash is valid or garbage data
/// </summary>
bool AnimationSet::CheckValid() const
{
	return headMarker == ANIMATION_SET_VALID_KEY && tailMarker == ANIMATION_SET_VALID_KEY;
}

/// <summary>
/// In order to help the receiving end, we can compute the total byte size of all the
/// animations in the set (excluding the set itself).
/// </summary>
int AnimationSet::ComputeAnimationTotalSize() const
{
	if (!CheckValid())
		return -1;

	int ret = 0;
	for (int i = 0; i < count; ++i)
	{
		ret += animations[i]->ComputeByteSize();
	}
	return ret;
}

/// <summary>
/// How many animations are in the set?
/// </summary>
uint32_t AnimationSet::Count() const
{
	if (CheckValid())
		return count;
	else
		return 0;
}

/// <summary>
/// Fetch an animation, if valid!
/// </summary>
const Animation* AnimationSet::GetAnimation(int index) const
{
	if (CheckValid() && index < count)
	{
		return animations[index];
	}
	else
	{
		return nullptr;
	}
}


//-----------------------------------------------------------------------------


/// <summary>
/// Constructor
/// </summary>
ReceiveAnimSetSM::ReceiveAnimSetSM()
	: count(0)
	, currentAnim(0)
	, currentState(State_Done)
	, currentAnimFlashAddress(0)
{
}

/// <summary>
/// Prepare to receive an animation set, erase the flash, etc...
/// </summary>
void ReceiveAnimSetSM::Setup(short animCount, short totalAnimByteSize)
{
	count = animCount;
	currentAnim = 0;
	currentState = State_ErasingFlash;
	currentAnimFlashAddress = ANIMATION_SET_ADDRESS;

	// This small array will keep track of the animation pointers we've
	// written to flash, so that we can then write them in the animation set itself!
	animationPtrInFlash = (Animation**)malloc(sizeof(Animation*) * count);

	// How many pages will we need?
	int totalSize = totalAnimByteSize + sizeof(AnimationSet);
	int pageCount = (totalSize + 1023) / 1024; // a page is 1k, and we want to round up!

	// Erase all necessary pages
	bool eraseSuccessful = true;
	for (int i = 0; i < pageCount; ++i)
	{
		if (flashPageErase(ANIMATION_SET_START_PAGE - i) != 0)
		{
			debugPrint("Not enough free pages (needed ");
			debugPrint(pageCount);
			debugPrint(" pages for ");
			debugPrint(totalSize);
			debugPrintln(" bytes of animation data)");
			eraseSuccessful = false;
			break;
		}
	}

	if (eraseSuccessful)
	{
		// Register for update so we can try to send ack messages
		die.RegisterUpdate(this, [](void* token)
		{
			((ReceiveAnimSetSM*)token)->Update();
		});
		currentState = State_SendingAck;
	}
	else
	{
		currentState = State_Done;
	}
}

/// <summary>
/// State machine update method, which we registered with the die in Setup()
/// </summary>
void ReceiveAnimSetSM::Update()
{
	switch (currentState)
	{
	case State_SendingAck:
		{
			if (die.SendMessage(DieMessage::MessageType_TransferAnimSetAck))
			{
				// Prepare to receive animation bulk data
				currentState = State_TransferAnim;
				receiveBulkDataSM.Setup();
			}
			// Else we try again next update
		}
		break;
	case State_TransferAnim:
		{
			// Is it done?
			if (receiveBulkDataSM.TransferComplete())
			{
				// The anim data is ready, copy it to flash!
				// The reason we're subtracting here is that we place animation set and animations in descending addresses
				currentAnimFlashAddress -= receiveBulkDataSM.mallocSize;
				Animation* dst = (Animation*)currentAnimFlashAddress;
				if (flashWriteBlock(dst, receiveBulkDataSM.mallocData, receiveBulkDataSM.mallocSize) != 0)
				{
					debugPrint("Could not write animation ");
					debugPrint(currentAnim);
					debugPrintln(" to flash");
					receiveBulkDataSM.Finish();
					Finish();
				}
				else
				{
					// Clean up memory allocated by the bulk transfer
					receiveBulkDataSM.Finish();

					// Remember the address of this new animation
					animationPtrInFlash[currentAnim] = dst;
					currentAnim++;

					if (currentAnim == count)
					{
						// No more anims to receive, program AnimationSet in flash
						// We overwrite it manually!
						uint32_t* progAnimationSetRaw = (uint32_t*)ANIMATION_SET_ADDRESS;
						flashWrite(progAnimationSetRaw, ANIMATION_SET_VALID_KEY);
						progAnimationSetRaw += sizeof(uint32_t);
						flashWriteBlock(progAnimationSetRaw, animationPtrInFlash, sizeof(Animation*) * count);
						progAnimationSetRaw += sizeof(Animation*) * MAX_ANIMATIONS;
						flashWrite(progAnimationSetRaw, count);
						progAnimationSetRaw += sizeof(uint32_t);
						flashWrite(progAnimationSetRaw, ANIMATION_SET_VALID_KEY);

						// Clean up animation table too
						Finish();
					}
					else
					{
						currentState = State_SendingReadyForNextAnim;
					}
				}
			}
			// Else keep waiting
		}
		break;
	case State_SendingReadyForNextAnim:
		{
			if (die.SendMessage(DieMessage::MessageType_TransferAnimReadyForNextAnim))
			{
				// Prepare to receive next animation bulk data
				currentState = State_TransferAnim;
				receiveBulkDataSM.Setup();
			}
			// Else we try again next update
		}
		break;
	default:
		break;
	}
}


/// <summary>
/// Clean up after having received a new animation set
/// This mostly means feeing temporary memory
/// </summary>
void ReceiveAnimSetSM::Finish()
{
	count = 0;
	currentAnim = 0;
	currentState = State_Done;
	currentAnimFlashAddress = 0;
	if (animationPtrInFlash != nullptr)
	{
		free(animationPtrInFlash);
	}
	animationPtrInFlash = nullptr;
	die.UnregisterUpdateToken(this);
}


//-----------------------------------------------------------------------------


/// <summary>
/// Constructor
/// </summary>
SendAnimSetSM::SendAnimSetSM()
	: currentAnim(0)
	, currentState(State_Done)
{
}

/// <summary>
/// Prepare for sending the animation set data over bluetooth
/// </summary>
void SendAnimSetSM::Setup()
{
	if (animationSet->CheckValid())
	{
		currentAnim = 0;
		currentState = State_SendingSetup;

		die.RegisterUpdate(this, [](void* token)
		{
			((SendAnimSetSM*)token)->Update();
		});
	}
}

/// <summary>
/// State Machine update method, which we registered with the die in Setup()
/// </summary>
void SendAnimSetSM::Update()
{
	switch (currentState)
	{
	case State_SendingSetup:
		{
			DieMessageTransferAnimSet setupMsg;
			setupMsg.count = animationSet->Count();
			setupMsg.totalAnimationByteSize = animationSet->ComputeAnimationTotalSize();
			if (die.SendMessage(&setupMsg, sizeof(setupMsg)))
			{
				die.RegisterMessageHandler(DieMessage::MessageType_TransferAnimSetAck, this, [](void* token, DieMessage* msg)
				{
					((SendAnimSetSM*)token)->currentState = State_SetupAckReceived;
				});

				currentState = State_WaitingForSetupAck;
			}
			// Else try again next update
		}
		break;
	case State_SetupAckReceived:
		{
			// Unregister from the ack message
			die.UnregisterMessageHandler(DieMessage::MessageType_TransferAnimSetAck);

			// Start transfering the anims
			auto anim = animationSet->GetAnimation(currentAnim);
			sendBulkDataSM.Setup((byte*)anim, anim->ComputeByteSize());
			currentState = State_SendingAnim;
		}
		break;
	case State_SendingAnim:
		{
			// Is the transfer complete?
			if (sendBulkDataSM.TransferComplete())
			{
				// Next anim!
				currentAnim++;
				if (currentAnim == animationSet->Count())
				{
					// We're done!
					Finish();
				}
				else
				{
					// Wait for a message indicating that the other side is ready for the next anim
					die.RegisterMessageHandler(DieMessage::MessageType_TransferAnimReadyForNextAnim, this, [](void* token, DieMessage* msg)
					{
						((SendAnimSetSM*)token)->currentState = State_ReceivedReadyForNextAnim;
					});

					currentState = State_WaitingForReadyForNextAnim;
				}
			}
			// Else wait some more
		}
		break;
	case SendAnimSetSM::State_ReceivedReadyForNextAnim:
		{
			// Unregister from the ack message
			die.UnregisterMessageHandler(DieMessage::MessageType_TransferAnimReadyForNextAnim);

			// Start transfering the anims
			auto anim = animationSet->GetAnimation(currentAnim);
			sendBulkDataSM.Setup((byte*)anim, anim->ComputeByteSize());
			currentState = State_SendingAnim;
		}
		break;
	default:
		break;
	}
}

/// <summary>
/// Clean up after ourselves
/// </summary>
void SendAnimSetSM::Finish()
{
	currentAnim = 0;
	currentState = State_Done;
	die.UnregisterUpdateToken(this);
}


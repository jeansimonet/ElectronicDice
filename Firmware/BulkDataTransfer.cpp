#include "BulkDataTransfer.h"
#include "Debug.h"
#include "BluetoothMessage.h"
#include "Die.h"

/// <summary>
/// Constructor
/// </summary>
SendBulkDataSM::SendBulkDataSM()
	: data(nullptr)
	, size(0)
	, currentState(State_Done)
	, currentOffset(0)
{
}

/// <summary>
/// Prepare for a bulk data transfer
/// </summary>
/// <param name="theData">The data we want to send</param>
/// <param name="theSize">The size of the data</param>
void SendBulkDataSM::Setup(const byte* theData, short theSize)
{
	data = theData;
	size = theSize;
	currentState = State_Setup;
	currentOffset = 0;

	die.RegisterUpdate(this, [](void* token)
	{
		((SendBulkDataSM*)token)->Update();
	});
}

/// <summary>
/// State machine update method
/// </summary>
void SendBulkDataSM::Update()
{
	switch (currentState)
	{
	case State_Setup:
		{
			// Send setup message over
			DieMessageBulkSetup setupMsg;
			setupMsg.size = size;

			// Register for SetupAck message
			die.RegisterMessageHandler(DieMessage::MessageType_BulkSetupAck, this, [](void* token, DieMessage* msg)
			{
				((SendBulkDataSM*)token)->currentState = State_SetupAckReceived;
			});

			// Send setup message
			// (we register for a response first to be sure and not miss the ack)
			if (die.SendMessage(&setupMsg, sizeof(setupMsg)))
			{
				currentState = State_WaitingForSetupAck;
			}
			// Else we try again!
		}
		break;
	case State_SetupAckReceived:
		// Unregister from setupack
		die.UnregisterMessageHandler(DieMessage::MessageType_BulkSetupAck);
		// Register for data ack
		die.RegisterMessageHandler(DieMessage::MessageType_BulkDataAck, this, [](void* token, DieMessage* msg)
		{
			((SendBulkDataSM*)token)->currentState = State_DataAckReceived;
		});
		// Voluntary fall through
	case State_DataAckReceived:
		{
			short remainingSize = min(size - currentOffset, 16);
			if (remainingSize > 0)
			{
				// Send next packet
				DieMessageBulkData dataMsg;
				dataMsg.size = min(size - currentOffset, 16);
				dataMsg.offset = currentOffset;
				memcpy(dataMsg.data, &data[currentOffset], dataMsg.size);

				// (we register for a response first to be sure and not miss the ack)
				if (die.SendMessage(&dataMsg, sizeof(dataMsg)))
				{
					currentState = State_WaitingForDataAck;
				}
				// Else we try again next update
			}
			else
			{
				// We're done!
				die.UnregisterMessageHandler(DieMessage::MessageType_BulkDataAck);
				currentState = State_TransferComplete;
			}

		}
		break;
	default:
		// Nothing
		break;
	}
}

/// <summary>
/// Are we done?
/// </summary>
bool SendBulkDataSM::TransferComplete()
{
	return currentState == State_TransferComplete;
}

/// <summary>
/// Clean up!
/// </summary>
void SendBulkDataSM::Finish()
{
	currentOffset = 0;
	currentState = State_Done;
	die.UnregisterUpdateToken(this);
}


//-----------------------------------------------------------------------------

/// <summary>
/// Constructor
/// </summary>
ReceiveBulkDataSM::ReceiveBulkDataSM()
	: mallocData(nullptr)
	, mallocSize(0)
	, currentState(State_Done)
	, currentTotalSize(0)
{
}

/// <summary>
/// Prepare to send bulk data to the phone
/// </summary>
void ReceiveBulkDataSM::Setup()
{
	if (mallocData != nullptr)
	{
		Finish();
	}
	mallocData = nullptr;
	mallocSize = 0;
	currentState = State_WaitingForSetup;
	currentTotalSize = 0;

	// Register handler
	die.RegisterMessageHandler(DieMessage::MessageType_BulkSetup, this, [](void* token, DieMessage* msg)
	{
		auto bulkSetupMsg = static_cast<DieMessageBulkSetup*>(msg);
		auto me = ((ReceiveBulkDataSM*)token);
		me->currentState = State_SetupReceived;
		me->mallocSize = bulkSetupMsg->size;
		me->mallocData = (byte*)malloc(bulkSetupMsg->size);
		if (me->mallocData == nullptr)
		{
			debugPrint("Malloc failed on ");
			debugPrint(bulkSetupMsg->size);
			debugPrintln(" bytes");
		}
	});

	die.RegisterUpdate(this, [](void* token)
	{
		((ReceiveBulkDataSM*)token)->Update();
	});
}

/// <summary>
/// State machine update
/// </summary>
void ReceiveBulkDataSM::Update()
{
	switch (currentState)
	{
	case State_SetupReceived:
		{
			// Unregister setup handler
			die.UnregisterMessageHandler(DieMessage::MessageType_BulkSetup);

			// Register handler for data
			die.RegisterMessageHandler(DieMessage::MessageType_BulkData, this, [](void* token, DieMessage* msg)
			{
				auto bulkDataMsg = static_cast<DieMessageBulkData*>(msg);
				auto me = ((ReceiveBulkDataSM*)token);
				memcpy(&me->mallocData[bulkDataMsg->offset], bulkDataMsg->data, bulkDataMsg->size);
				me->currentState = State_DataReceived;
				me->currentTotalSize += bulkDataMsg->size;
			});

			// Acknowledge the setup
			if (die.SendMessage(DieMessage::MessageType_BulkSetupAck))
			{
				currentState = State_WaitingForData;
			}
			// Else we try again next update
		}
		break;
	case State_DataReceived:
		{
			if (die.SendMessage(DieMessage::MessageType_BulkDataAck))
			{
				if (currentTotalSize == mallocSize)
				{
					// We're done
					die.UnregisterMessageHandler(DieMessage::MessageType_BulkData);
					currentState = State_TransferComplete;
				}
				else
				{
					currentState = State_WaitingForData;
				}
			}
			// Else we try again next update
		}
		break;
	default:
		break;
	}
}

/// <summary>
/// Are we done?
/// </summary>
bool ReceiveBulkDataSM::TransferComplete()
{
	return currentState == State_TransferComplete;
}

/// <summary>
/// Clean up!
/// </summary>
void ReceiveBulkDataSM::Finish()
{
	if (mallocData != nullptr)
	{
		free(mallocData);
		mallocData = nullptr;
	}
	mallocSize = 0;
	currentState = State_Done;

	die.UnregisterUpdateToken(this);
}


// BluetoothMessage.h

#ifndef _BLUETOOTHMESSAGE_h
#define _BLUETOOTHMESSAGE_h

#include "Arduino.h"

/// <summary>
///  Base class for messages from the die to the app
/// </summary>
struct DieMessage
{
	enum MessageType : byte
	{
		MessageType_None = 0,
		MessageType_State,
		MessageType_Telemetry,
		MessageType_BulkSetup,
		MessageType_BulkSetupAck,
		MessageType_BulkData,
		MessageType_BulkDataAck,
		MessageType_TransferAnimSet,
		MessageType_TransferAnimSetAck,
		MessageType_TransferAnimReadyForNextAnim,
		MessageType_TransferSettings,
		MessageType_TransferSettingsAck,
		MessageType_DebugLog,

		MessageType_PlayAnim,
		MessageType_RequestState,
		MessageType_RequestAnimSet,
		MessageType_RequestSettings,
		MessageType_RequestTelemetry,
		MessateType_ProgramDefaultAnimSet,
		MessageType_Count
	};

	MessageType type;

	inline DieMessage(MessageType msgType) : type(msgType) {}
	static const char* GetMessageTypeString(MessageType msgType);

protected:
	inline DieMessage() : type(MessageType_None) {}
};

/// <summary>
/// Describes a face up detection message
/// </summary>
struct DieMessageState
	: public DieMessage
{
	byte state;

	inline DieMessageState() : DieMessage(DieMessage::MessageType_State) {}
};

/// <summary>
/// Describes an acceleration readings message (for telemetry)
/// </summary>
struct DieMessageAcc
	: public DieMessage
{
	struct AccelFrame
	{
		short x;
		short y;
		short z;
		short deltaTime;
	};

	AccelFrame data[2];

	inline DieMessageAcc() : DieMessage(DieMessage::MessageType_Telemetry) {}
};

struct DieMessageBulkSetup
	: DieMessage
{
	short size;

	inline DieMessageBulkSetup() : DieMessage(DieMessage::MessageType_BulkSetup) {}
};

struct DieMessageBulkData
	: DieMessage
{
	byte size;
	short offset;
	byte data[16];

	inline DieMessageBulkData() : DieMessage(DieMessage::MessageType_BulkData) {}
};

struct DieMessageTransferAnimSet
	: DieMessage
{
	byte count;
	short totalAnimationByteSize;

	inline DieMessageTransferAnimSet() : DieMessage(DieMessage::MessageType_TransferAnimSet) {}
};

#pragma pack(push, 1)
struct DieMessageDebugLog
	: public DieMessage
{
	char text[19];

	inline DieMessageDebugLog() : DieMessage(DieMessage::MessageType_DebugLog) {}
};
#pragma pack(pop)

struct DieMessagePlayAnim
	: public DieMessage
{
	byte animation;

	inline DieMessagePlayAnim() : DieMessage(DieMessage::MessageType_PlayAnim) {}
};

struct DieMessageRequestTelemetry
	: public DieMessage
{
	bool telemetry;

	inline DieMessageRequestTelemetry() : DieMessage(DieMessage::MessageType_RequestTelemetry) {}
};

struct DieMessageProgramDefaultAnimSet
	: public DieMessage
{
	uint32_t color;

	inline DieMessageProgramDefaultAnimSet() : DieMessage(DieMessage::MessateType_ProgramDefaultAnimSet) {}
};
#endif


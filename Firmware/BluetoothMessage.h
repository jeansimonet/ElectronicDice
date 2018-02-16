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
		MessageType_Face,
		MessageType_Telemetry
	};

	MessageType type;
};

/// <summary>
/// Describes a face up detection message
/// </summary>
struct DieMessageFace
	: public DieMessage
{
	byte face;
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
};

#endif


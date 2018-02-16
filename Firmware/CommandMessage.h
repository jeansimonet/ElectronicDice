// CommandMessage.h

#ifndef _COMMANDMESSAGE_h
#define _COMMANDMESSAGE_h

#include "Arduino.h"

/// <summary>
/// Base class for messages from the app to the die
/// </summary>
struct CommandMessage
{
	enum CommandType : byte
	{
		CommandType_None = 0,
		CommandType_PlayAnim,
		CommandType_SetName,
		CommandType_PrintName,
		CommandType_DumpEEPROM,
	};

	CommandType type;
};

/// <summary>
/// Describes a command to play a specific animation
/// </summary>
struct CommandMessagePlayAnim
	: public CommandMessage
{
	byte animation;
};

struct CommandMessageSetName
	: public CommandMessage
{
	char name[17];
};

struct CommandMessagePrintName
	: public CommandMessage
{
	// No extra data
};

struct CommandMessageDumpEEPROM
	: public CommandMessage
{
	// No extra data
};

#endif


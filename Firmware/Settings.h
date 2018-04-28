// DiceSettings.h

#ifndef _DICESETTINGS_h
#define _DICESETTINGS_h

#include "Arduino.h"
#include "BulkDataTransfer.h"

#define SETTINGS_PAGE (251)
#define SETTINGS_VALID_KEY (0x05E77165) // 0SETTINGS in leet speak ;)
#define SETTINGS_ADDRESS (SETTINGS_PAGE * 1024)

class Settings
{
public:
	// Indicates whether there is valid data
	uint32_t headMarker;
	char name[16];
	uint32_t tailMarker;

	bool CheckValid() const;

	static bool EraseSettings();
	static bool TransferSettings(Settings* sourceSettings);
	static bool TransferSettingsRaw(void* rawData, size_t rawDataSize);
};

extern const Settings* settings;

/// <summary>
/// This defines a state machine that can manage receiving the dice settings over bluetooth
/// and then update them in flash
/// </summary>
class ReceiveSettingsSM
{
private:
	enum State
	{
		State_ErasingFlash = 0,
		State_SendingAck,
		State_TransferSettings,
		State_Done
	};

	State currentState;
	ReceiveBulkDataSM receiveBulkDataSM;

	typedef void(*FinishedCallback)(void* token);
	FinishedCallback FinishedCallbackHandler;
	void* FinishedCallbackToken;

private:
	void Finish();

public:
	ReceiveSettingsSM();
	void Setup(void* token, FinishedCallback handler);
	void Update();
};

/// <summary>
/// This defines a state machine that can send the current settings over
/// bluetooth to the phone. Typically so the phone can edit it and redownload it.
/// </summary>
class SendSettingsSM
{
private:
	enum State
	{
		State_SendingSetup,
		State_WaitingForSetupAck,
		State_SetupAckReceived,
		State_SendingSettings,
		State_Done
	};

	State currentState;

	// Temporarily stores animation pointers as we program them in flash
	SendBulkDataSM sendBulkDataSM;

	typedef void(*FinishedCallback)(void* token);
	FinishedCallback FinishedCallbackHandler;
	void* FinishedCallbackToken;

private:
	void Finish();

public:
	SendSettingsSM();
	void Setup(void* token, FinishedCallback handler);
	void Update();
};

#endif


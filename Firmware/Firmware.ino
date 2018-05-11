//----------------
// !!!IMPORTANT!!!
//----------------
// Make sure to include ota_bootloader.h so it 
// includes the bootloader hex in the image.
#include "Rainbow.h"
#include <ota_bootloader.h>
#include "BLEConsole.h"
#include <SimbleeBLE.h>
//----------------
// !!!IMPORTANT!!!
//----------------

#include "HWTesting.h"
#include "Die.h"
#include "LEDs.h"

Die die;

#define TEST_NONE			0
#define TEST_SERIAL			1
#define TEST_I2C			2
#define TEST_ACC			3
#define TEST_LED			4
#define TEST_LED_SLOW		5
#define TEST_LED_POWER		6
#define TEST_BLE			7
#define TEST_SLEEP			8
#define TEST_SLEEP_ACC		9
#define TEST_ACC_DICE		10
#define TEST_TIMER			11
#define TEST_LED_DICE		12
#define TEST_SETTINGS		13
#define TEST_ANIMATION_SET	14
#define TEST_ANIMATIONS		15
#define TEST_BATTERY		17
#define TEST_CHARGING		16
#define TEST_BATT_DISCHARGE 18
#define TEST_ALL_CONNECTIONS 19

//#define TEST TEST_LED_SLOW
//#define TEST TEST_LED

#define POWERPIN	4
#define NUMPIXELS	21
#define DATAPIN		30
#define CLOCKPIN	29
#define CHARGING_PIN 22

void setup()
{
	// Turn off LED power
	pinMode(DATAPIN, OUTPUT);
	pinMode(CLOCKPIN, OUTPUT);
	pinMode(POWERPIN, OUTPUT);
	digitalWrite(DATAPIN, LOW);
	digitalWrite(CLOCKPIN, LOW);
	digitalWrite(POWERPIN, HIGH);

	//----------------
	// !!!IMPORTANT!!!
	//----------------
	// Always start BLE Stack first, so that we get a chance to perform DFU
	SimbleeBLE.advertisementData = "Dice_Boot";
	SimbleeBLE.deviceName = "Dice_Boot";
	SimbleeBLE.txPowerLevel = 4;
	SimbleeBLE.begin();
	delay(3000);
	//----------------
	// !!!IMPORTANT!!!
	//----------------

#if (TEST != 0)
	SetupTest(TEST);
#else
	die.init();
#endif
}

void SimbleeBLE_onConnect()
{
#if (TEST == 0)
	die.onConnect();
#endif
}

void SimbleeBLE_onDisconnect()
{
#if (TEST == 0)
	die.onDisconnect();
#endif
}

void SimbleeBLE_onReceive(char *data, int len)
{
#if (TEST == 0)
	die.onReceive(data, len);
#endif
}

void loop()
{
#if (TEST == 0)
	die.update();
#else
	UpdateTest(TEST);
#endif
}

void SetupTest(int testIndex)
{
	switch (testIndex)
	{
	case TEST_SERIAL:
		Tests::TestSerial();
		break;
	case TEST_I2C:
		Tests::TestI2C();
		break;
	case TEST_ACC:
		Tests::TestAcc();
		break;
	case TEST_LED:
		Tests::TestLED();
		break;
	case TEST_LED_SLOW:
		Tests::TestLEDSlow();
		break;
	case TEST_LED_POWER:
		Tests::TestLEDPower();
		break;
	case TEST_BLE:
		// Nothing yet!
		break;
	case TEST_SLEEP:
		Tests::TestSleepForever();
		break;
	case TEST_SLEEP_ACC:
		Tests::TestSleepAwakeAcc();
		break;
	case TEST_ACC_DICE:
		Tests::TestAccDice();
		break;
	case TEST_TIMER:
		Tests::TestTimerSetup();
		break;
	case TEST_LED_DICE:
		Tests::TestLEDDice();
		break;
	case TEST_SETTINGS:
		Tests::TestSettings();
		break;
	case TEST_ANIMATION_SET:
		Tests::TestAnimationSet();
		break;
	case TEST_ANIMATIONS:
		Tests::TestAnimationsSetup();
		break;
	case TEST_BATTERY:
		Tests::TestBattery();
		break;
	case TEST_CHARGING:
		Tests::TestCharging();
		break;
	case TEST_BATT_DISCHARGE:
		Tests::TestBatteryDischarge();
		break;
	case TEST_ALL_CONNECTIONS:
		Tests::TestAllHardwareConnections();
		break;
	default:
		break;
	}
}

void UpdateTest(int testIndex)
{
	switch (testIndex)
	{
	case TEST_TIMER:
		Tests::TestTimerUpdate();
		break;
	case TEST_ANIMATIONS:
		Tests::TestAnimationsUpdate();
		break;
	default:
		break;
	}
}


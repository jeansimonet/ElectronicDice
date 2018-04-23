//----------------
// !!!IMPORTANT!!!
//----------------
// Make sure to include ota_bootloader.h so it 
// includes the bootloader hex in the image.
//#include <ota_bootloader.h>
#include <SimbleeBLE.h>
//----------------
// !!!IMPORTANT!!!
//----------------

#include "HWTesting.h"
#include "Die.h"

Die die;

//#define TEST_SERIAL
//#define TEST_I2C
//#define TEST_ACC
#define TEST_LED 
//#define TEST_LED_POWER
//#define TEST_BLE
//#define TEST_SLEEP

#if defined(TEST_SERIAL) || \
	defined(TEST_I2C) || \
	defined(TEST_ACC) || \
	defined(TEST_LED) || \
	defined(TEST_LED_POWER) || \
	defined(TEST_BLE) || \
	defined(TEST_SLEEP)
#define NO_DICE
#endif

void setup()
{
	//----------------
	// !!!IMPORTANT!!!
	//----------------
	// Always start BLE Stack first, so that we get a chance to perform DFU
	SimbleeBLE.advertisementData = "Dice_Boot";
	SimbleeBLE.deviceName = "Dice_Boot";
	SimbleeBLE.txPowerLevel = 4;
	SimbleeBLE.begin();
	delay(1000);
	//----------------
	// !!!IMPORTANT!!!
	//----------------

#if defined(TEST_SERIAL)
	Tests::TestSerial();
#elif defined(TEST_I2C)
	Tests::TestI2C();
#elif defined(TEST_ACC)
	Tests::TestAcc();
#elif defined(TEST_LED)
	Tests::TestLED();
#elif defined(TEST_LED_POWER)
	Tests::TestLEDPower();
#elif defined(TEST_BLE)
	// Nothing yet!
#elif defined(TEST_SLEEP)
	Tests::TestSleepForever();
#else
	die.init();
#endif
}

void SimbleeBLE_onConnect()
{
#if !defined(NO_DICE)
	die.onConnect();
#endif
}

void SimbleeBLE_onDisconnect()
{
#if !defined(NO_DICE)
	die.onDisconnect();
#endif
}

void SimbleeBLE_onReceive(char *data, int len)
{
#if !defined(NO_DICE)
	die.onReceive(data, len);
#endif
}

void loop()
{
#if !defined(NO_DICE)
	die.update();
#endif
}

//----------------
// !!!IMPORTANT!!!
//----------------
// Make sure to include ota_bootloader.h so it 
// includes the bootloader hex in the image.
#include <ota_bootloader.h>
#include <SimbleeBLE.h>
//----------------
// !!!IMPORTANT!!!
//----------------

#include "HWTesting.h"
#include "Die.h"

Die die;

//#define TEST_I2C
//#define TEST_ACC
//#define TEST_LED 
//#define TEST_BLE


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
	//----------------
	// !!!IMPORTANT!!!
	//----------------

#if defined(TEST_I2C)
	Tests::TestI2C();
#elif defined(TEST_ACC)
	Tests::TestAcc();
#elif defined(TEST_LED)
	Tests::TestLED();
#elif defined(TEST_BLE)
#else
	die.init();
#endif
}

void SimbleeBLE_onConnect()
{
#if !defined(TEST_I2C) && !defined(TEST_ACC) && !defined(TEST_LED) && !defined(TEST_BLE)
	die.onConnect();
#endif
}

void SimbleeBLE_onDisconnect()
{
#if !defined(TEST_I2C) && !defined(TEST_ACC) && !defined(TEST_LED) && !defined(TEST_BLE)
	die.onDisconnect();
#endif
}

void SimbleeBLE_onReceive(char *data, int len)
{
#if !defined(TEST_I2C) && !defined(TEST_ACC) && !defined(TEST_LED) && !defined(TEST_BLE)
	die.onReceive(data, len);
#endif
}

void loop()
{
#if !defined(TEST_I2C) && !defined(TEST_ACC) && !defined(TEST_LED) && !defined(TEST_BLE)
	die.update();
#endif
}

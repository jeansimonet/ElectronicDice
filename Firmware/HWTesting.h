#ifndef _HWTESTING_h
#define _HWTESTING_h
#include "Arduino.h"

namespace Tests
{
	void TestSerial();
	void TestI2C();
	void TestAcc();
	void TestLED();
	void TestLEDPower();
	void TestSleepForever();
	void TestSleepAwakeAcc();
}

#endif


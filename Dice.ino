//#include "Dice_Accel.h"
//#include "DiceWire.h"
//#include "Dice_LED.h"
//#include "OTA_Bootloader.h"
//
//#include <SimbleeBLE.h>
//
#include "LEDAnimationController.h"
#include "RotatingSix.h"
#include "LEDAnimations.h"
#include "DiceDebug.h"
#include "DiceTimer.h"
#include "LEDController.h"
#include "LEDAnimation.h"
#include "LEDAnimations.h"

#define pin 29
//
//
//int determineFace(float x, float y, float z)
//{
//	if (abs(x) > abs(y))
//	{
//		if (abs(x) > abs(z))
//		{
//			// X is greatest direction
//			if (x > 0)
//			{
//				return 5;
//			}
//			else
//			{
//				return 2;
//			}
//		}
//		else
//		{
//			// Z is greatest direction
//			if (z > 0)
//			{
//				return 3;
//			}
//			else
//			{
//				return 4;
//			}
//		}
//	}
//	else
//	{
//		if (abs(z) > abs(y))
//		{
//			// Z is greatest direction
//			if (z > 0)
//			{
//				return 3;
//			}
//			else
//			{
//				return 4;
//			}
//		}
//		else
//		{
//			// Y is greatest direction
//			if (y > 0)
//			{
//				return 1;
//			}
//			else
//			{
//				return 6;
//			}
//		}
//	}
//}
//
//void setup()
//{
//	//SimbleeBLE.begin();
//	//ota_bootloader_start(); //begins OTA enabled state
//	DiceWire.begin();
//	//Serial.begin(115200);
//	LEDs.Init();
//	DiceAccel.init();
//}
//
//
//int currentFace = 0;
//int currentLED = 0;
//void loop()
//{
//	for (int i = 0; i <= 100; ++i)
//	{
//		for (int j = 0; j < 1; ++j)
//		{
//			LEDs.Set(5, 4);
//			delayMicroseconds(i * 100);
//			LEDs.Clear();
//			delayMicroseconds((100 - i) * 100);
//		}
//	}
//
//	for (int i = 100; i >= 0; --i)
//	{
//		for (int j = 0; j < 1; ++j)
//		{
//			LEDs.Set(5, 4);
//			delayMicroseconds(i * 100);
//			LEDs.Clear();
//			delayMicroseconds((100 - i) * 100);
//		}
//	}
//}

//https://github.com/finnurtorfa/nrf51/blob/master/lib/nrf51sdk/Nordic/nrf51822/Board/nrf6310/timer_example/main.c

#include "LEDAnimation.h"
#include <SimbleeBLE.h>
#include "DiceWire.h"
#include "DiceLED.h"
#include "LEDAnimation.h"

void setup()
{
	// put your setup code here, to run once:
	//setup I2C on the pins of your choice
	override_uart_limit = true;
	//diceDebug.begin();
	DiceWire.begin();
	LEDs.init();
	ledController.begin();
	animationController.begin();
	diceTimer.begin();

	//ledController.setLED(0, 1);
	//ledController.setLED(1, 1);
	//ledController.setLED(2, 1);
	//ledController.setLED(3, 1);
	//ledController.setLED(4, 1);
	//ledController.setLED(5, 1);
	//ledController.setLED(6, 1);
	//ledController.setLED(7, 1);
	//ledController.setLED(8, 1);
	//ledController.setLED(9, 1);
	//ledController.setLED(10, 1);
	//ledController.setLED(11, 1);
	//ledController.setLED(12, 1);
	//ledController.setLED(13, 1);
	//ledController.setLED(14, 1);
	//ledController.setLED(15, 1);
	//ledController.setLED(16, 1);
	//ledController.setLED(17, 1);
	//ledController.setLED(18, 1);
	//ledController.setLED(19, 1);
	//ledController.setLED(20, 1);

	// put your setup code here, to run once:
	// Set buzzer pin as output!
	pinMode(pin, OUTPUT);

	// start the BLE stack
	// put your setup code here, to run once:
	SimbleeBLE.advertisementData = "Dice";
	SimbleeBLE.deviceName = "Dice";

	SimbleeBLE.txPowerLevel = 0;
	SimbleeBLE.begin();

	if (!diceDebug.isDebugOn())
	{
		pinMode(0, OUTPUT);
		pinMode(1, OUTPUT);
		digitalWrite(0, LOW);
		digitalWrite(1, LOW);
	}

	//animationController.play(&(ledAnimations.FaceOneSlowPulse));
	//animationController.play(&(ledAnimations.FaceSixFastRotatingPulses));

	//int ms = millis();
	//for (int i = 0; i < 100; +i)
	//{
	//	//ledController.printAllLeds();
	//	animationController.update(ms);
	//	ms += 33;
	//}
	//animationController.play(&rotatingSix);

}

void SimbleeBLE_onConnect()
{
	// Insert code
	diceDebug.println("Connected!");

}

void SimbleeBLE_onDisconnect()
{
	// Insert code here
	diceDebug.println("Disconnected!");
}

void loop()
{
	//Serial.println("loop");
	//animationController.update();
	//delay(1000);
	//delay(3000);
	//animationController.play(&(ledAnimations.FaceOneSlowPulse));
	delay(3000);
	animationController.play(&rotatingSix);
	//for (int i = 0; i < 256; i += 2)
	//{
	//	delay(10);
	//	ledController.setLED(5, i);
	//}
	//for (int i = 254; i >= 0; i -= 2)
	//{
	//	delay(10);
	//	ledController.setLED(5, i);
	//}
	//delay(3000);
	//for (int i = 1; i < 20; i += 1)
	//{
	//	delay(100);
	//	NRF_TIMER2->TASKS_STOP;
	//	NRF_TIMER2->CC[0] = i;
	//	NRF_TIMER2->TASKS_CLEAR;
	//	NRF_TIMER2->TASKS_START;
	//}
	//for (int i = 20; i >= 1; i -= 1)
	//{
	//	delay(100);
	//	NRF_TIMER2->TASKS_STOP;
	//	NRF_TIMER2->CC[0] = i;
	//	NRF_TIMER2->TASKS_CLEAR;
	//	NRF_TIMER2->TASKS_START;
	//}
}

void updateLEDs()
{

}


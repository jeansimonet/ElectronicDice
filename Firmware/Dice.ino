#include "DiceController.h"
#include "RandomLEDs.h"
#include "DiceCommands.h"
#include <SimbleeBLE.h>
#include "DiceWire.h"
#include "DiceLED.h"
#include "LEDAnimationController.h"
#include "RotatingLEDs.h"
#include "LEDAnimations.h"
#include "DiceDebug.h"
#include "DiceTimer.h"
#include "LEDController.h"
#include "LEDAnimation.h"
#include "LEDAnimations.h"
#include "DiceAccel.h"
#include "MessageQueue.h"
#include "AccelController.h"

#define pin 29

int currentFace = 0;
void setup()
{
	// start the BLE stack
	// put your setup code here, to run once:
	SimbleeBLE.advertisementData = "Alice";
	SimbleeBLE.deviceName = "Alice";

	SimbleeBLE.txPowerLevel = 4;
	SimbleeBLE.begin();

	// put your setup code here, to run once:
	//setup I2C on the pins of your choice
	diceDebug.begin();
	diceWire.begin();
	LEDs.init(); // Depends on I2C
	ledController.begin(); // Uses LEDs
	animationController.begin(); // Talks to controller
	diceAccel.init();
	accelController.begin();
	diceControl.init();
	messageQueue.init();
	diceTimer.begin(); // Kicks off the timers!
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

void SimbleeBLE_onReceive(char *data, int len)
{
	// First 4 bytes are a command code
	if (len >= 4)
	{
		ProcessCommand(data, len);
	}
}

void updateFaceAnimation()
{
	int newFace = accelController.currentFace();
	if (newFace != currentFace)
	{
		if (!SimbleeBLE_radioActive)
		{
			currentFace = newFace;

			// Toggle leds
			animationController.stopAll();
			switch (currentFace)
			{
			case 0:
				animationController.play(&ledAnimations.FaceOneSlowPulse);
				break;
			case 1:
				animationController.play(&ledAnimations.rotatingTwo);
				break;
			case 2:
				animationController.play(&ledAnimations.rotatingThree);
				break;
			case 3:
				animationController.play(&ledAnimations.rotatingFour);
				break;
			case 4:
				animationController.play(&ledAnimations.FaceFiveCross);
				break;
			case 5:
				animationController.play(&ledAnimations.rotatingSix);
				break;
			}
			diceDebug.print("sending face number ");
			diceDebug.println(currentFace);
			SimbleeBLE.sendByte(currentFace);
		}
	}
}

void loop()
{
	messageQueue.update();
	updateFaceAnimation();
}


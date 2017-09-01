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
	SimbleeBLE.advertisementData = "Dice";
	SimbleeBLE.deviceName = "Dice";

	SimbleeBLE.txPowerLevel = 0;
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
	messageQueue.init();

	// Set buzzer pin as output!
	pinMode(pin, OUTPUT);

	if (!diceDebug.isDebugOn())
	{
		pinMode(0, OUTPUT);
		pinMode(1, OUTPUT);
		digitalWrite(0, LOW);
		digitalWrite(1, LOW);
	}

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

void updateFaceAnimation()
{
	int newFace = accelController.currentFace();
	if (newFace != currentFace)
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
	}
}

void loop()
{
	messageQueue.update();
}


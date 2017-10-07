// LEDAnimations.h

#ifndef _LEDANIMATIONS_h
#define _LEDANIMATIONS_h

#include "Arduino.h"
#include "LEDAnimation.h"
#include "RotatingLEDs.h"
#include "RandomLEDs.h"

class LEDAnimations
{
public:
	Animation FaceOneSlowPulse;
	Animation FaceSixSlowPulse;
	Animation FaceFiveCross;

	RotatingLEDs rotatingSix;
	RotatingLEDs rotatingFour;
	RotatingLEDs rotatingThree;
	RotatingLEDs rotatingTwo;

	RandomLEDs randomLEDs;

public:
	LEDAnimations();
};

extern const Curve constantOn;
extern const Curve constantOff;
extern const Curve rampUpDown;
extern const Curve on128Off128;

extern LEDAnimations ledAnimations;

#endif


// LEDAnimations.h

#ifndef _LEDANIMATIONS_h
#define _LEDANIMATIONS_h

#include "Arduino.h"
#include "Animation.h"
#include "RotatingLEDs.h"
#include "RandomLEDs.h"

namespace LEDAnimations
{
	extern Animation FaceOneSlowPulse;
	extern Animation FaceSixSlowPulse;
	extern Animation FaceFiveCross;

	extern RotatingLEDs rotatingSix;
	extern RotatingLEDs rotatingFour;
	extern RotatingLEDs rotatingThree;
	extern RotatingLEDs rotatingTwo;

	extern RandomLEDs randomLEDs;

	extern const Curve constantOn;
	extern const Curve constantOff;
	extern const Curve rampUpDown;
	extern const Curve rampUpDown128;
	extern const Curve rampUpDown128x2;
	extern const Curve rampUpDown128x3;
	extern const Curve on128Off128;
	extern const RGBCurve whiteRampUpDown128;
}

#endif


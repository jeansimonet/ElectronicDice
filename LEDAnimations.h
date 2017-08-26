// LEDAnimations.h

#ifndef _LEDANIMATIONS_h
#define _LEDANIMATIONS_h

#include "Arduino.h"
#include "LEDAnimation.h"

class LEDAnimations
{
public:
	Animation FaceOneSlowPulse;
	Animation FaceSixSlowPulse;
	Animation FaceSixFastRotatingPulses;

public:
	LEDAnimations();
};

extern Curve constantOn;
extern Curve constantOff;
extern Curve rampUpDown;
extern Curve on128Off128;

extern LEDAnimations ledAnimations;

#endif


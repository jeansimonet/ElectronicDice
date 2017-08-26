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

extern LEDAnimations ledAnimations;

#endif


#ifndef _IANIMATION_h
#define _IANIMATION_h

#include "Arduino.h"

/// <summary>
/// Base class for animations. Some animations use curves, others are more
/// programmatic (like the random animation for instance)
/// </summary>
class IAnimation
{
public:
	virtual void start() = 0;
	virtual int updateLEDs(int time, int retIndices[], uint32_t retColors[]) = 0;
	virtual int stop(int retIndices[]) = 0;
	virtual int totalDuration() = 0;
};

#endif


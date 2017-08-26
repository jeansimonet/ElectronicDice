// 
// 
// 

#include "LEDAnimations.h"

LEDAnimations ledAnimations;

#define MAKE_FROM_FLOAT(x) ((int)(255 * (x)))

Curve constantOn =
{
	// Keyframes
	{
		{ 0, 255 },
		{ 255, 255 }
	},
	// Number of keyframes
	2
};

Curve constantOff =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 255, 0 }
	},
	// Number of keyframes
	2
};

Curve rampUpDown =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 127, 255 },
		{ 255, 0 },
	},
	// Number of keyframes
	3
};

Curve rampUpDownRepeat3 =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 127, 255 },
		{ 255, 0 },
	},
	// Number of keyframes
	3
};

Curve on128Off128 =
{
	// Keyframes
	{
		{ 0,	255 },
		{ 127,	255 },
		{ 128,	0 },
		{ 255,	0 }
	},
	// Number of keyframes
	4
};


LEDAnimations::LEDAnimations()
{
	FaceOneSlowPulse.addTrack(0, 0, 0, 3000, &rampUpDown);

	FaceSixSlowPulse.addTrack(5, 0, 0, 3000, &rampUpDown);
	FaceSixSlowPulse.addTrack(5, 1, 0, 3000, &rampUpDown);
	FaceSixSlowPulse.addTrack(5, 2, 0, 3000, &rampUpDown);
	FaceSixSlowPulse.addTrack(5, 3, 0, 3000, &rampUpDown);
	FaceSixSlowPulse.addTrack(5, 4, 0, 3000, &rampUpDown);
	FaceSixSlowPulse.addTrack(5, 5, 0, 3000, &rampUpDown);

	FaceSixFastRotatingPulses.addTrack(5, 0,   0, 500, &rampUpDown);
	FaceSixFastRotatingPulses.addTrack(5, 1, 100, 500, &rampUpDown);
	FaceSixFastRotatingPulses.addTrack(5, 2, 200, 500, &rampUpDown);
	FaceSixFastRotatingPulses.addTrack(5, 5, 300, 500, &rampUpDown);
	FaceSixFastRotatingPulses.addTrack(5, 4, 400, 500, &rampUpDown);
	FaceSixFastRotatingPulses.addTrack(5, 3, 500, 500, &rampUpDown);
}

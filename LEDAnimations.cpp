// 
// 
// 

#include "LEDAnimations.h"

LEDAnimations ledAnimations;

#define MAKE_FROM_FLOAT(x) ((int)(255 * (x)))

const Curve constantOn =
{
	// Keyframes
	{
		{ 0, 255 },
		{ 255, 255 }
	},
	// Number of keyframes
	2
};

const Curve constantOff =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 255, 0 }
	},
	// Number of keyframes
	2
};

const Curve rampUpDown =
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

const Curve rampUpDown128 =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 127, 128 },
		{ 255, 0 },
	},
	// Number of keyframes
	3
};

const Curve rampUpDown128x2 =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 63, 128 },
		{ 127, 0 },
		{ 183, 128 },
		{ 255, 0 },
	},
	// Number of keyframes
	5
};

const Curve rampUpDown128x3 =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 42, 128 },
		{ 85, 0 },
		{ 127, 128 },
		{ 170, 0 },
		{ 212, 128 },
		{ 255, 0 },
	},
	// Number of keyframes
	5
};

const Curve on128Off128 =
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


int rotatingSixLEDs[] = { 0, 1, 2, 5, 4, 3 };
int rotatingFourLEDs[] = { 0, 1, 2, 3 };
int rotatingThreeLEDs[] = { 0, 2, 1, 2 };
int rotatingTwoLEDs[] = { 0, 1 };

LEDAnimations::LEDAnimations()
	: rotatingSix(100, 500, 3, 5, rotatingSixLEDs, sizeof(rotatingSixLEDs) / sizeof(rotatingSixLEDs[0]))
	, rotatingFour(200, 350, 4, 3, rotatingFourLEDs, sizeof(rotatingFourLEDs) / sizeof(rotatingFourLEDs[0]))
	, rotatingThree(200, 500, 4, 2, rotatingThreeLEDs, sizeof(rotatingThreeLEDs) / sizeof(rotatingThreeLEDs[0]))
	, rotatingTwo(200, 350, 5, 1, rotatingTwoLEDs, sizeof(rotatingTwoLEDs) / sizeof(rotatingTwoLEDs[0]))
	, randomLEDs(5000, 300, 50)
{
	FaceOneSlowPulse.addTrack(0, 0, 0, 3000, &rampUpDown);

	FaceSixSlowPulse.addTrack(5, 0, 0, 3000, &rampUpDown128);
	FaceSixSlowPulse.addTrack(5, 1, 0, 3000, &rampUpDown128);
	FaceSixSlowPulse.addTrack(5, 2, 0, 3000, &rampUpDown128);
	FaceSixSlowPulse.addTrack(5, 3, 0, 3000, &rampUpDown128);
	FaceSixSlowPulse.addTrack(5, 4, 0, 3000, &rampUpDown128);
	FaceSixSlowPulse.addTrack(5, 5, 0, 3000, &rampUpDown128);

	FaceFiveCross.addTrack(4, 0, 0, 3000, &rampUpDown128x3);
	FaceFiveCross.addTrack(4, 1, 0, 3000, &rampUpDown128x3);
	FaceFiveCross.addTrack(4, 4, 0, 3000, &rampUpDown128x3);
	FaceFiveCross.addTrack(4, 3, 0, 3000, &rampUpDown128x3);
	FaceFiveCross.addTrack(4, 2, 500, 2000, &rampUpDown128x2);
}

#include "LEDAnimations.h"
#include "AnimController.h"

using namespace LEDAnimations;

#define MAKE_FROM_FLOAT(x) ((int)(255 * (x)))

const Curve LEDAnimations::constantOn =
{
	// Keyframes
	{
		{ 0, 255 },
		{ 255, 255 }
	},
	// Number of keyframes
	2
};

const Curve LEDAnimations::constantOff =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 255, 0 }
	},
	// Number of keyframes
	2
};

const Curve LEDAnimations::rampUpDown =
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

const Curve LEDAnimations::rampUpDown128 =
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

const Curve LEDAnimations::rampUpDown128x2 =
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

const Curve LEDAnimations::rampUpDown128x3 =
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

const Curve LEDAnimations::on128Off128 =
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

const RGBCurve LEDAnimations::whiteRampUpDown128 =
{
	// Keyframes
	{
		{ 0, 0 },
		{ 127, 0x808080 },
		{ 255, 0 },
	},
	// Number of keyframes
	3
};


int rotatingSixLEDs[] = { 0, 1, 2, 5, 4, 3 };
RotatingLEDs LEDAnimations::rotatingSix(0x00FFFFFF, 100, 500, 3, 5, rotatingSixLEDs, 6);

int rotatingFourLEDs[] = { 0, 1, 2, 3 };
RotatingLEDs LEDAnimations::rotatingFour(0x00FFFFFF, 200, 350, 4, 3, rotatingFourLEDs, 4);

int rotatingThreeLEDs[] = { 0, 2, 1, 2 };
RotatingLEDs LEDAnimations::rotatingThree(0x00FFFFFF, 200, 500, 4, 2, rotatingThreeLEDs, 4);

int rotatingTwoLEDs[] = { 0, 1 };
RotatingLEDs LEDAnimations::rotatingTwo(0x00FFFFFF, 200, 350, 5, 1, rotatingTwoLEDs, 2);

Animation LEDAnimations::FaceOneSlowPulse(AnimationTrack(0, 0, 0, 3000, &whiteRampUpDown128));

Animation LEDAnimations::FaceSixSlowPulse(
	AnimationTrack(5, 0, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(5, 1, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(5, 2, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(5, 3, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(5, 4, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(5, 5, 0, 3000, &whiteRampUpDown128)
);

Animation LEDAnimations::FaceFiveCross(
	AnimationTrack(4, 0, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(4, 1, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(4, 4, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(4, 3, 0, 3000, &whiteRampUpDown128),
	AnimationTrack(4, 2, 500, 2000, &whiteRampUpDown128)
);

RandomLEDs LEDAnimations::randomLEDs;

//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
//uint32_t Wheel(byte WheelPos) {
//	WheelPos = 255 - WheelPos;
//	if (WheelPos < 85) {
//		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//	}
//	if (WheelPos < 170) {
//		WheelPos -= 85;
//		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//	}
//	WheelPos -= 170;
//	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//}
//
//// Slightly different, this makes the rainbow equally distributed throughout
//void rainbowCycle(uint8_t wait) {
//	uint16_t i, j;
//
//	for (j = 0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
//		for (i = 0; i< strip.numPixels(); i++) {
//			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//		}
//		strip.show();
//		delay(wait);
//	}
//}
//
//void rainbow(uint8_t wait) {
//	uint16_t i, j;
//
//	for (j = 0; j<256; j++) {
//		for (i = 0; i<strip.numPixels(); i++) {
//			strip.setPixelColor(i, Wheel((i + j) & 255));
//		}
//		strip.show();
//		delay(wait);
//	}
//}

#include "Animation.h"
#include "LEDs.h"
#include "Utils.h"

#define MAX_LEVEL (256)

using namespace Core;

/// <summary>
/// Dims the passed in color by the passed in intensity (normalized 0 - 255)
/// </summary>
uint32_t scaleColor(uint32_t refColor, byte intensity)
{
	byte r = getRed(refColor);
	byte g = getGreen(refColor);
	byte b = getBlue(refColor);
	return toColor(r * intensity / MAX_LEVEL, g * intensity / MAX_LEVEL, b * intensity / MAX_LEVEL);
}

uint32_t Curve::evaluate(uint32_t refColor, int time) const
{
	if (count == 0)
		return 0;

	// Find the first keyframe
	int nextIndex = 0;
	while (nextIndex < count && keyframes[nextIndex].time < time)
	{
		nextIndex++;
	}

	byte intensity = 0;
	if (nextIndex == 0)
	{
		// The first keyframe is already after the requested time, clamp to first value
		intensity = keyframes[nextIndex].intensity;
	}
	else if (nextIndex == count)
	{
		// The last keyframe is still before the requested time, clamp to the last value
		intensity = keyframes[nextIndex - 1].intensity;
	}
	else
	{
		// Grab the prev and next keyframes
		auto& nextKeyframe = keyframes[nextIndex];
		auto& prevKeyframe = keyframes[nextIndex - 1];

		// Compute the interpolation parameter
		// To stick to integer math, we'll scale the values
		int scaler = 1024;
		int scaledPercent = (time - prevKeyframe.time) * scaler / (nextKeyframe.time - prevKeyframe.time);
		int scaledIntensity = prevKeyframe.intensity * (scaler - scaledPercent) + nextKeyframe.intensity * scaledPercent;
		intensity = scaledIntensity / scaler;
	}

	return scaleColor(refColor, intensity);
}

/// <summary>
/// Returns the keyframe's color in uint32_t type!
/// </summary>
uint32_t RGBKeyframe::color() const
{
	return toColor(red, green, blue);
}

/// <summary>
/// Generates a color keyframe from a single channel keyframe and a color
/// </summary>
RGBKeyframe RGBKeyframe::fromColorAndKeyframe(uint32_t color, const Keyframe& keyframe)
{
	RGBKeyframe ret;
	ret.time = keyframe.time;
	ret.red = (int)keyframe.intensity * getRed(color) / MAX_LEVEL;
	ret.green = (int)keyframe.intensity * getGreen(color) / MAX_LEVEL;
	ret.blue = (int)keyframe.intensity * getBlue(color) / MAX_LEVEL;
	return ret;
}

/// <summary>
/// Generates a color curve from a single channel curve and a color
/// </summary>
RGBCurve RGBCurve::fromColorAndCurve(uint32_t color, const Curve& curve)
{
	RGBCurve ret;
	ret.count = curve.count;
	// Convert each keyframe!
	for (int i = 0; i < ret.count; ++i)
	{
		ret.keyframes[i] = RGBKeyframe::fromColorAndKeyframe(color, curve.keyframes[i]);
	}
	return ret;
}

/// /// <summary>
/// Interpolate between keyframes of an animation curve
/// </summary>
/// <param name="time">The normalized time (0 - 255)</param>
/// <returns>The normalized intensity (0 - 255)</returns>
uint32_t RGBCurve::evaluate(int time) const
{
	if (count == 0)
		return 0;

	// Find the first keyframe
	int nextIndex = 0;
	while (nextIndex < count && keyframes[nextIndex].time < time)
	{
		nextIndex++;
	}

	if (nextIndex == 0)
	{
		// The first keyframe is already after the requested time, clamp to first value
		return keyframes[nextIndex].color();
	}
	else if (nextIndex == count)
	{
		// The last keyframe is still before the requested time, clamp to the last value
		return keyframes[nextIndex - 1].color();
	}
	else
	{
		// Grab the prev and next keyframes
		auto& nextKeyframe = keyframes[nextIndex];
		auto& prevKeyframe = keyframes[nextIndex - 1];

		// Compute the interpolation parameter
		// To stick to integer math, we'll scale the values
		int scaler = 1024;
		int scaledPercent = (time - prevKeyframe.time) * scaler / (nextKeyframe.time - prevKeyframe.time);
		int scaledRed = prevKeyframe.red * (scaler - scaledPercent) + nextKeyframe.red * scaledPercent;
		int scaledGreen = prevKeyframe.green * (scaler - scaledPercent) + nextKeyframe.green * scaledPercent;
		int scaledBlue = prevKeyframe.blue * (scaler - scaledPercent) + nextKeyframe.blue * scaledPercent;
		return toColor(scaledRed / scaler, scaledGreen / scaler, scaledBlue / scaler);
	}
}

/// <summary>
/// Default constructor
/// </summary>
AnimationTrack::AnimationTrack()
: face(0)
, index(0)
, startTime(0)
, duration(0)
, curve(nullptr)
{
}

/// <summary>
/// Initializing constructor
/// </summary>
AnimationTrack::AnimationTrack(int f, int i, int s, int d, const RGBCurve* c)
	: face(f)
	, index(i)
	, startTime(s)
	, duration(d)
	, curve(c)
{
}

/// <summary>
/// Helper method that returns a track's LED index, based on face and index-in-face
/// </summary>
int AnimationTrack::ledIndex() const
{
	return LEDs::ledIndex(face, index);
}

/// <summary>
/// Evaluate an animation track's intensity for a given time, in milliseconds.
/// Values outside the track's range are clamped to first or last keyframe value.
/// </summary>
/// <returns>A normalized intensity (0-255)</returns>
uint32_t AnimationTrack::evaluate(int time) const
{
	uint32_t ret = 0;
	if (time < startTime)
	{
		ret = curve->evaluate(0);
	}
	else if (time >= startTime + duration)
	{
		ret = curve->evaluate(256);
	}
	else
	{
		int scaler = MAX_LEVEL;
		int scaledTime = (time - startTime) * scaler / duration;
		ret = curve->evaluate(scaledTime);
	}

	// Scale the return value
	return ret;
}

/// <summary>
/// Constructor
/// </summary>
Animation::Animation()
	: count(0)
	, duration(0)
{
}

Animation::Animation(const AnimationTrack& track0)
{
	tracks[0] = track0;
	count = 1;
	duration = track0.duration + track0.startTime;
}

Animation::Animation(const AnimationTrack& track0, const AnimationTrack& track1)
{
	tracks[0] = track0;
	tracks[1] = track1;
	count = 2;
	duration = max(track0.duration + track0.startTime, track1.duration + track1.startTime);
}

Animation::Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2)
{
	tracks[0] = track0;
	tracks[1] = track1;
	tracks[2] = track2;
	count = 3;
	duration = max(max(
		track0.duration + track0.startTime,
		track1.duration + track1.startTime),
		track2.duration + track2.startTime);
}

Animation::Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2, const AnimationTrack& track3)
{
	tracks[0] = track0;
	tracks[1] = track1;
	tracks[2] = track2;
	tracks[3] = track3;
	count = 4;
	duration = max(max(max(
		track0.duration + track0.startTime,
		track1.duration + track1.startTime),
		track2.duration + track2.startTime),
		track3.duration + track3.startTime);
}

Animation::Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2, const AnimationTrack& track3, const AnimationTrack& track4)
{
	tracks[0] = track0;
	tracks[1] = track1;
	tracks[2] = track2;
	tracks[3] = track3;
	tracks[4] = track4;
	count = 5;
	duration = max(max(max(max(
		track0.duration + track0.startTime,
		track1.duration + track1.startTime),
		track2.duration + track2.startTime),
		track3.duration + track3.startTime),
		track4.duration + track4.startTime);
}

Animation::Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2, const AnimationTrack& track3, const AnimationTrack& track4, const AnimationTrack& track5)
{
	tracks[0] = track0;
	tracks[1] = track1;
	tracks[2] = track2;
	tracks[3] = track3;
	tracks[4] = track4;
	tracks[5] = track5;
	count = 6;
	duration = max(max(max(max(max(
		track0.duration + track0.startTime,
		track1.duration + track1.startTime),
		track2.duration + track2.startTime),
		track3.duration + track3.startTime),
		track4.duration + track4.startTime),
		track5.duration + track5.startTime);
}

/// <summary>
/// Kick off the animation
/// </summary>
void Animation::start()
{
	// Nothing to do here!
}

/// <summary>
/// Computes the list of LEDs that need to be on, and what their intensities should be
/// based on the different tracks of this animation.
/// </summary>
/// <param name="time">The animation time (in milliseconds)</param>
/// <param name="retIndices">the return list of LED indices to fill, max size should be at least 21, the total number of leds</param>
/// <param name="retColors">the return list of LED color to fill, max size should be at least 21, the total number of leds</param>
/// <returns>The number of leds/intensities added to the return array</returns>
int Animation::updateLEDs(int time, int retIndices[], uint32_t retColors[])
{
	for (int i = 0; i < count; ++i)
	{
		retIndices[i] = tracks[i].ledIndex();
		retColors[i] = tracks[i].evaluate(time);
	}
	return count;
}

/// <summary>
/// Clear all LEDs controlled by this animation, for instance when
/// the anim gets interrupted.
/// </summary>
int Animation::stop(int retIndices[])
{
	for (int i = 0; i < count; ++i)
	{
		retIndices[i] = tracks[i].ledIndex();
	}
	return count;
}

/// <summary>
/// Returns the duration of this animation
/// </summary>
int Animation::totalDuration()
{
	return duration;
}


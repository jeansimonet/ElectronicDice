// 
// 
// 

#include "LEDAnimation.h"
#include "DiceLED.h"
#include "LEDController.h"

#define MAX_LEVEL (256)

int Curve::evaluate(int time) const
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
		return keyframes[nextIndex].intensity;
	}
	else if (nextIndex == count)
	{
		// The last keyframe is still before the requested time, clamp to the last value
		return keyframes[nextIndex - 1].intensity;
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
		int scaledResult = prevKeyframe.intensity * (scaler - scaledPercent) + nextKeyframe.intensity * scaledPercent;
		return scaledResult / scaler;
	}
}


int AnimationTrack::ledIndex() const
{
	return LEDs.ledIndex(face, index);
}


int AnimationTrack::evaluate(int time) const
{
	int base = 0;
	if (time < startTime)
	{
		base = curve->evaluate(0);
	}
	else if (time >= startTime + duration)
	{
		base = curve->evaluate(256);
	}
	else
	{
		int scaler = MAX_LEVEL;
		int scaledTime = (time - startTime) * scaler / duration;
		base = curve->evaluate(scaledTime);
	}

	// Scale the return value
	return base;
}

Animation::Animation()
	: count(0)
	, duration(0)
{
}

void Animation::addTrack(const AnimationTrack& track)
{
	if (count < MAX_TRACKS)
	{
		tracks[count] = track;
		count++;
		if (track.duration + track.startTime > duration)
		{
			duration = track.duration + track.startTime;
		}
	}
}

void Animation::addTrack(int face, int index, int startTime, int trackDuration, const Curve* curve)
{
	if (count < MAX_TRACKS)
	{
		auto& track = tracks[count];
		track.face = face;
		track.index = index;
		track.startTime = startTime;
		track.duration = trackDuration;
		track.curve = curve;
		count++;
		if (track.duration + track.startTime > duration)
		{
			duration = track.duration + track.startTime;
		}
	}
}

void Animation::start()
{
	// Nothing to do here!
}

int Animation::updateLEDs(int time, int retIndices[], int retIntensities[])
{
	for (int i = 0; i < count; ++i)
	{
		retIndices[i] = tracks[i].ledIndex();
		retIntensities[i] = tracks[i].evaluate(time);
	}
	return count;
}

void Animation::clearLEDs()
{
	for (int i = 0; i < count; ++i)
	{
		ledController.setLED(tracks[i].ledIndex(), 0);
	}
}

int Animation::totalDuration()
{
	return duration;
}


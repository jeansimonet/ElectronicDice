// 
// 
// 

#include "LEDAnimation.h"
#include "LEDController.h"
#include "Dice_LED.h"
#include "Dice_Timer.h"
#include "Dice_Debug.h"

#define MAX_TIME (256)
#define RESOLUTION (33333) // 33.333 ms = 30 Hz

AnimationController animationController;

int Curve::evaluate(int time)
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


int AnimationTrack::ledIndex()
{
	return LEDs.ledIndex(face, index);
}


int AnimationTrack::evaluate(int time)
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
		int scaler = MAX_TIME;
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

void Animation::addTrack(int face, int index, int startTime, int trackDuration, Curve* curve)
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


void Animation::updateLEDs(int time)
{
	for (int i = 0; i < count; ++i)
	{
		int ledIndex = tracks[i].ledIndex();
		int ledIntensity = tracks[i].evaluate(time);
		ledController.setLED(ledIndex, ledIntensity);
	}
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

AnimationController::AnimInstance::AnimInstance()
	: animation(nullptr)
	, startTime(0)
{
}

int AnimationController::update()
{
	//digitalWrite(1, HIGH);
	int ms = millis();
	for (int i = 0; i < count; ++i)
	{
		auto& anim = animations[i];
		int animTime = ms - anim.startTime;
		if (animTime > anim.animation->totalDuration())
		{
			// The animation is over, get rid of it!
			stopAtIndex(i);

			// Decrement loop counter since we just replaced the current anim
			i--;
		}
		else
		{
			// Update the leds
			anim.animation->updateLEDs(animTime);
		}
	}

	//digitalWrite(1, LOW);
	return RESOLUTION;
}

// To be passed to the timer
int AnimationController::animationControllerUpdate()
{
	return animationController.update();
}

AnimationController::AnimationController()
	: count(0)
{
}

void AnimationController::begin()
{
	diceTimer.hook(RESOLUTION, animationControllerUpdate); // 33.333 ms = 30 Hz
}

void AnimationController::stop()
{
	diceTimer.unHook(animationControllerUpdate);
}

void AnimationController::play(Animation* anim)
{
	int prevAnimIndex = 0;
	for (; prevAnimIndex < count; ++prevAnimIndex)
	{
		if (animations[prevAnimIndex].animation == anim)
		{
			break;
		}
	}

	int ms = millis();
	if (prevAnimIndex < count)
	{
		// Replace a previous animation
		animations[prevAnimIndex].startTime = ms;
	}
	else if (count < MAX_ANIMS)
	{
		// Add a new animation
		animations[count].animation = anim;
		animations[count].startTime = ms;
		count++;
	}
	// Else there is no more room
}

void AnimationController::stop(Animation* anim)
{
	int prevAnimIndex = 0;
	for (; prevAnimIndex < count; ++prevAnimIndex)
	{
		if (animations[prevAnimIndex].animation == anim)
		{
			break;
		}
	}

	if (prevAnimIndex < count)
	{
		stopAtIndex(prevAnimIndex);
	}
	// Else the animation isn't playing
}

void AnimationController::stopAll()
{
	for (int i = 0; i < count; ++i)
	{
		animations[i].animation = nullptr;
		animations[i].startTime = 0;
	}
	count = 0;
	ledController.clearAll();
}

void AnimationController::stopAtIndex(int animIndex)
{
	// Found the animation, start by killing the leds it controls
	animations[animIndex].animation->clearLEDs();

	// Shift the other animations
	for (; animIndex < count - 1; ++animIndex)
	{
		animations[animIndex] = animations[animIndex + 1];
	}

	// Reduce the count
	count--;
}
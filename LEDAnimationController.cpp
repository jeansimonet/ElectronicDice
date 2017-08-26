// 
// 
// 

#include "LEDAnimationController.h"
#include "LEDAnimation.h"
#include "LEDController.h"
#include "Dice_LED.h"
#include "Dice_Timer.h"
#include "Dice_Debug.h"

#define RESOLUTION (33333) // 33.333 ms = 30 Hz

AnimationController animationController;


AnimationController::AnimInstance::AnimInstance()
	: animation(nullptr)
	, startTime(0)
{
}

int AnimationController::update()
{
	update(millis());
}

void AnimationController::update(int ms)
{
	if (count > 0)
	{
		digitalWrite(1, HIGH);
	}
	else
	{
		digitalWrite(1, LOW);
	}

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
			int ledIndices[MAX_TRACKS];
			int intensities[MAX_TRACKS];
			int ledCount = anim.animation->updateLEDs(animTime, ledIndices, intensities);
			ledController.setLEDs(ledIndices, intensities, ledCount);
		}
	}

	//digitalWrite(1, LOW);
}

// To be passed to the timer
void AnimationController::animationControllerUpdate()
{
	animationController.update();
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

void AnimationController::play(IAnimation* anim)
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
		animations[prevAnimIndex].animation->clearLEDs();
		animations[prevAnimIndex].animation->start();
	}
	else if (count < MAX_ANIMS)
	{
		// Add a new animation
		animations[count].animation = anim;
		animations[count].startTime = ms;
		animations[count].animation->start();
		count++;
	}
	// Else there is no more room
}

void AnimationController::stop(IAnimation* anim)
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
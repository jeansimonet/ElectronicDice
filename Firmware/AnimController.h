// LEDAnimationController.h

#ifndef _LEDANIMATIONCONTROLLER_h
#define _LEDANIMATIONCONTROLLER_h

#include "Arduino.h"
#include "IAnimation.h"

#define MAX_ANIMS (5)

/// <summary>
/// Manages a set of running animations, talking to the LED controller
/// to tell it what LEDs must have what intensity at what time.
/// </summary>
class AnimController
{
private:
	/// <summary>
	/// Internal helper struct used to store a running animation instance
	/// </summary>
	struct AnimInstance
	{
		IAnimation* animation;
		int startTime; //ms

		AnimInstance();
	};

	// Our currently running animations
	AnimInstance animations[MAX_ANIMS];
	int count;

private:
	void stopAtIndex(int animIndex);
	void removeAtIndex(int animIndex);
	int update();
	void update(int ms);

	// To be passed to the timer
	static void animationControllerUpdate(void* param);

public:
	AnimController();
	void begin();
	void stop();
	void play(IAnimation* anim);
	void stop(IAnimation* anim);
	void stopAll();
};
#endif


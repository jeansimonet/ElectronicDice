// LEDAnimationController.h

#ifndef _LEDANIMATIONCONTROLLER_h
#define _LEDANIMATIONCONTROLLER_h

#include "Arduino.h"

#define MAX_ANIMS (5)

class IAnimation;

class AnimationController
{
private:
	struct AnimInstance
	{
		AnimInstance();
		IAnimation* animation;
		int startTime; //ms
	};

	AnimInstance animations[MAX_ANIMS];
	int count;

private:
	void stopAtIndex(int animIndex);
	int update();
	void update(int ms);

	// To be passed to the timer
	static void animationControllerUpdate();

public:
	AnimationController();
	void begin();
	void stop();
	void play(IAnimation* anim);
	void stop(IAnimation* anim);
	void stopAll();
};

extern AnimationController animationController;


#endif


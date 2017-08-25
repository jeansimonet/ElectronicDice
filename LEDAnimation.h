// LEDAnimation.h

#ifndef _LEDANIMATION_h
#define _LEDANIMATION_h

#include "Arduino.h"

#define MAX_KEYFRAMES (16)
#define MAX_TRACKS (6)
#define MAX_ANIMS (5)

struct Keyframe
{
public:
	byte time;		// 0 - 255 (normalized)
	byte intensity;	// 0 - 255 (normalized)
};

struct Curve
{
public:
	Keyframe keyframes[MAX_KEYFRAMES];
	int count;

	int evaluate(int time); // 0 - 255 (normalized time)
};

struct AnimationTrack
{
public:
	int face;		// 0 - 5 (1 through 6)
	int index;		// 0 - face (depends on the face)
	int startTime;	// ms
	int duration;	// ms
	Curve* curve;	// Typically this comes from an array of pre-defined curves

	int evaluate(int time);
	int ledIndex();
};

struct Animation
{
private:
	AnimationTrack tracks[MAX_TRACKS];
	int count;
	int duration; // ms

public:
	Animation();
	void addTrack(const AnimationTrack& track);
	void addTrack(int face, int index, int startTime, int duration, Curve* curve);
	void updateLEDs(int time);
	void clearLEDs();
	int totalDuration();
};

class AnimationController
{
private:
	struct AnimInstance
	{
		AnimInstance();
		Animation* animation;
		int startTime; //ms
	};

	AnimInstance animations[MAX_ANIMS];
	int count;

private:
	void stopAtIndex(int animIndex);
	int update();

	// To be passed to the timer
	static int animationControllerUpdate();

public:
	AnimationController();
	void begin();
	void stop();
	void play(Animation* anim);
	void stop(Animation* anim);
	void stopAll();
};

extern AnimationController animationController;

#endif


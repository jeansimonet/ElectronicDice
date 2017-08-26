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

class IAnimation
{
public:
	virtual void start();
	virtual int updateLEDs(int time, int retIndices[], int retIntensities[]) = 0;
	virtual void clearLEDs() = 0;
	virtual int totalDuration() = 0;
};

class Animation
	: public IAnimation
{
private:
	AnimationTrack tracks[MAX_TRACKS];
	int count;
	int duration; // ms

public:
	Animation();
	void addTrack(const AnimationTrack& track);
	void addTrack(int face, int index, int startTime, int duration, Curve* curve);
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], int retIntensities[]) override;
	virtual void clearLEDs() override;
	virtual int totalDuration() override;
};


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


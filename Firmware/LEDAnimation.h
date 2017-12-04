// LEDAnimation.h

#ifndef _LEDANIMATION_h
#define _LEDANIMATION_h

#include "Arduino.h"

#define MAX_KEYFRAMES (16)
#define MAX_TRACKS (6)

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

	int evaluate(int time) const; // 0 - 255 (normalized time)
};

struct AnimationTrack
{
public:
	int face;		// 0 - 5 (1 through 6)
	int index;		// 0 - face (depends on the face)
	int startTime;	// ms
	int duration;	// ms
	const Curve* curve;	// Typically this comes from an array of pre-defined curves

	int evaluate(int time) const;
	int ledIndex() const;
};

class IAnimation
{
public:
	virtual void start() = 0;
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
	void addTrack(int face, int index, int startTime, int duration, const Curve* curve);
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], int retIntensities[]) override;
	virtual void clearLEDs() override;
	virtual int totalDuration() override;
};


#endif


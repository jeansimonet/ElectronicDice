// KeyframedLEDs.h

#ifndef _KEYFRAMEDLEDS_h
#define _KEYFRAMEDLEDS_h

#include "Arduino.h"
#include "IAnimation.h"

#define MAX_KEYFRAMES (16)
#define MAX_TRACKS (6)

/// <summary>
/// Simple single channel keyframe
/// </summary>
struct Keyframe
{
public:
	byte time;
	byte intensity;
};

/// <summary>
/// Single channel curve
/// </summary>
struct Curve
{
public:
	Keyframe keyframes[MAX_KEYFRAMES];
	int count;

	byte evaluate(int time) const; // 0 - 255 (normalized time)
	uint32_t evaluate(uint32_t refColor, int time) const; // 0 - 255 (normalized time)
};

/// <summary>
/// Stores a single keyframe of an LED animation
/// </summary>
struct RGBKeyframe
{
public:
	byte time;		// 0 - 255 (normalized)
	byte red;		// 0 - 255 (normalized)
	byte green;
	byte blue;

	uint32_t color() const;
	static RGBKeyframe fromColorAndKeyframe(uint32_t color, const Keyframe& keyframe);
};

/// <summary>
/// Stores a set of LED animation keyframes and can interpolate between them
/// </summary>
struct RGBCurve
{
public:
	RGBKeyframe keyframes[MAX_KEYFRAMES];
	int count;

	uint32_t evaluate(int time) const; // 0 - 255 (normalized time)

	static RGBCurve fromColorAndCurve(uint32_t color, const Curve& keyframe);
};

/// <summary>
/// An animation track is essentially a scaled animation curve for a
/// specific LED. It defines how long the curve is stretched over and when it starts.
/// </summary>
struct AnimationTrack
{
public:
	int face;		// 0 - 5 (1 through 6)
	int index;		// 0 - face (depends on the face)
	int startTime;	// ms
	int duration;	// ms
	const RGBCurve* curve;	// Typically this comes from an array of pre-defined curves

	uint32_t evaluate(int time) const;
	int ledIndex() const;

	AnimationTrack();
	AnimationTrack(int face, int index, int startTime, int duration, const RGBCurve* curve);
};

/// <summary>
/// A keyframe-based animation
/// </summary>
class Animation
	: public IAnimation
{
private:
	AnimationTrack tracks[MAX_TRACKS];
	int count;
	int duration; // ms

public:
	Animation();
	Animation(const AnimationTrack& track0);
	Animation(const AnimationTrack& track0, const AnimationTrack& track1);
	Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2);
	Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2, const AnimationTrack& track3);
	Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2, const AnimationTrack& track3, const AnimationTrack& track4);
	Animation(const AnimationTrack& track0, const AnimationTrack& track1, const AnimationTrack& track2, const AnimationTrack& track3, const AnimationTrack& track4, const AnimationTrack& track5);

	// Interface implementation
	virtual void start() override;
	virtual int updateLEDs(int time, int retIndices[], uint32_t retColors[]) override;
	virtual int stop(int retIndices[]) override;
	virtual int totalDuration() override;
};



#endif


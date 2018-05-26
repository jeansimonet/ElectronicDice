#include "SimpleThrowDetector.h"
#include "Accelerometer.h"
#include "Debug.h"
#include "Die.h"
#include "AnimController.h"

using namespace Systems;
using namespace Devices;

SimpleThrowDetector Systems::simpleThrowDetector;

#define SIGMA_DECAY_START (0.95f)
#define SIGMA_DECAY_STOP (0.05f)
#define SIGMA_THRESHOLD_START (100)
#define SIGMA_THRESHOLD_END (0.5)
#define ROLL_MIN_TIME (300) //ms
#define FACE_THRESHOLD (0.85f)

void SimpleThrowDetector::begin()
{
	sigma = 0.0f;
	currentState = ThrowState_OnFace;
	rollStartTime = millis();
	accelController.hook(accelControllerCallback, nullptr);
}

void SimpleThrowDetector::accelControllerCallback(void* ignore, const AccelFrame& frame)
{
	simpleThrowDetector.onAccelFrame(frame);
}

void SimpleThrowDetector::onAccelFrame(const AccelFrame& frame)
{
	// Add the last frame
	float jerkX = accelerometer.convert(frame.jerkX);
	float jerkY = accelerometer.convert(frame.jerkY);
	float jerkZ = accelerometer.convert(frame.jerkZ);
	float jerk2 = jerkX * jerkX + jerkY * jerkY + jerkZ * jerkZ;
	//debugPrint("Sigma: ");
	//debugPrintln(sigma);

	switch (currentState)
	{
	case ThrowState_StartedRolling:
		sigma = sigma * SIGMA_DECAY_START + jerk2 * (1.0f - SIGMA_DECAY_START);
		if (sigma < SIGMA_THRESHOLD_END)
		{
			// Stopped rolling, so this is like a small bump, not enough
			currentState = ThrowState_OnFace;
		}
		else
		{
			if (millis() > rollStartTime + ROLL_MIN_TIME)
			{
				// Ok, it's been long enough
				currentState = ThrowState_RolledLongEnough;
			}
			// Keep waiting
		}
		break;
	case ThrowState_RolledLongEnough:
		sigma = sigma * SIGMA_DECAY_STOP + jerk2 * (1.0f - SIGMA_DECAY_STOP);
		if (sigma < SIGMA_THRESHOLD_END)
		{
			currentState = ThrowState_OnFace;
			onFaceFace = accelController.currentFace();
			if (abs(accelerometer.convert(frame.X)) > FACE_THRESHOLD ||
				abs(accelerometer.convert(frame.Y)) > FACE_THRESHOLD ||
				abs(accelerometer.convert(frame.Z)) > FACE_THRESHOLD)
			{
				// We stopped moving
				// Play an anim, and switch state
				int animIndex = onFaceFace + 6; // hardcoded for now
				animController.play(animationSet->GetAnimation(animIndex));
				die.playAnimation(animIndex);
			}
		}
		break;
	case ThrowState_OnFace:
		sigma = sigma * SIGMA_DECAY_START + jerk2 * (1.0f - SIGMA_DECAY_START);
		if (sigma >= SIGMA_THRESHOLD_START)
		{
			// We started moving, count time
			rollStartTime = millis();
			currentState = ThrowState_StartedRolling;
		}
		// Else nothing
		break;
	}
}




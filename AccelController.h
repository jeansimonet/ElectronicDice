// AccelController.h

#ifndef _ACCELCONTROLLER_h
#define _ACCELCONTROLLER_h

#include "Arduino.h"

class AccelController
{
private:
	int face;

private:

	// To be passed to the timer
	static void accelControllerUpdate();
	static void updateCurrentFace();
	int determineFace(float x, float y, float z);

public:
	AccelController();
	void begin();
	void stop();

	void update();
	int currentFace();
};

extern AccelController accelController;

#endif


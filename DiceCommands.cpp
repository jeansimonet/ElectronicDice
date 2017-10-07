// 
// 
// 

#include "DiceCommands.h"
#include "LEDAnimationController.h"
#include "LEDAnimations.h"

void ProcessCommand(char* data, int len)
{
	// Parse command
	uint32_t command = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

	// Move pointer
	len -= 4;
	data += 4;

	switch (command)
	{
	case CMD_PLAY_ANIM:
		{
			if (len == 1)
			{
				PlayAnim(data[0]);
			}
		}
		break;
	case CMD_BUZZER:
		break;
	}
}

void PlayAnim(int animIndex)
{
	switch (animIndex)
	{
	case 0:
		animationController.play(&ledAnimations.FaceOneSlowPulse);
		break;
	case 1:
		animationController.play(&ledAnimations.rotatingTwo);
		break;
	case 2:
		animationController.play(&ledAnimations.rotatingThree);
		break;
	case 3:
		animationController.play(&ledAnimations.rotatingFour);
		break;
	case 4:
		animationController.play(&ledAnimations.FaceFiveCross);
		break;
	case 5:
		animationController.play(&ledAnimations.rotatingSix);
		break;
	case 6:
		animationController.play(&ledAnimations.randomLEDs);
	}
}
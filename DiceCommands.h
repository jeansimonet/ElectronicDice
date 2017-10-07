// DiceCommands.h

#ifndef _DICECOMMANDS_h
#define _DICECOMMANDS_h

#include "Arduino.h"

#define CMD_PLAY_ANIM ('A' << 24 | 'N' << 16 | 'I' << 8 | 'M')
#define CMD_BUZZER ('B' << 24 | 'U' << 16 | 'Z' << 8 | 'Z')

void ProcessCommand(char* data, int len);
void PlayAnim(int animIndex);

#endif


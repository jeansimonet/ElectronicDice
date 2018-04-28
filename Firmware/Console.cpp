#include "Console.h"
#include "Utils.h"
#include "Die.h"
#include "LEDs.h"

#if defined(_CONSOLE)

using namespace Systems;
using namespace Core;

Console Systems::console;

/// <summary>
/// Initialize the console
/// </summary>
void Console::begin()
{
	override_uart_limit = true;
	Serial.begin(9600);
	Serial.println("Console on");
}

int Console::available()
{
	return Serial.available();
}

size_t Console::readBytesUntil(char terminator, char *buffer, size_t length)
{
	Serial.readBytesUntil(terminator, buffer, length);
}

const char Keyword_PlayAnim[] = "playanim";
const char Keyword_SetName[] = "name";
const char Keyword_ClearLEDs[] = "clearleds";
const char Keyword_SetLED[] = "setled";

const char Keyword_Help[] = "help";

/// <summary>
/// Processes a console command, parsing the command name and parameters
/// </summary>
/// <param name="data">The command buffer</param>
/// <param name="len">The length of the command buffer</param>
void Console::processCommand(char* data, int len)
{
	char commandWord[20] = "";
	int commandLength = parseWord(data, len, commandWord, 20);
	if (commandLength > 0)
	{
		if (strcmp(commandWord, Keyword_PlayAnim) == 0)
		{
			// Parse animation number
			char number[4] = "";
			int numberLength = parseWord(data, len, number, 4);
			if (numberLength > 0)
			{
				// Convert the number and run the animation!
				int animNumber = atoi(number);
				die.playAnimation(animNumber);
			}
		}
		else if (strcmp(commandWord, Keyword_ClearLEDs) == 0)
		{
			leds.clearAll();
		}
		else if (strcmp(commandWord, Keyword_SetLED) == 0)
		{
			// Parse number of face
			char face[4] = "";
			int faceLength = parseWord(data, len, face, 4);
			if (faceLength > 0)
			{
				char led[4] = "";
				int ledLength = parseWord(data, len, led, 4);
				if (ledLength > 0)
				{
					char color[10] = "";
					int colorLength = parseWord(data, len, color, 10);
					if (colorLength)
					{
						int f = atoi(face);
						int l = atoi(led);
						uint32_t c = strtol(color, nullptr, 16);
						leds.setLED(f, l, c);
					}
				}
			}
		}
		else if (strcmp(commandWord, Keyword_Help) == 0)
		{
			println("Possible commands:");
			println("  playanim <number> - Plays one of the face animations");
			println("  clearleds - Turns all LEDs off");
			println("  setled <face> <index> <color> - Sets the given LED to the passed in color");
		}
		else
		{
			print("Unknown command \'");
			print(commandWord);
			println("\', type help for list of available commands");
		}
	}
}

void Console::print(const char string[])
{
	Serial.print(string);
}

void Console::print(char c)
{
	Serial.print(c);
}

void Console::print(unsigned char c, int encoding)
{
	Serial.print(c, encoding);
}

void Console::print(int i, int encoding)
{
	Serial.print(i, encoding);
}

void Console::print(unsigned int i, int encoding)
{
	Serial.print(i, encoding);
}

void Console::print(long l, int encoding)
{
	Serial.print(l, encoding);
}

void Console::print(unsigned long l, int encoding)
{
	Serial.print(l, encoding);
}

void Console::print(double d, int precision)
{
	Serial.print(d, precision);
}

void Console::println(const char string[])
{
	Serial.println(string);
}

void Console::println(char c)
{
	Serial.println(c);
}

void Console::println(unsigned char c, int encoding)
{
	Serial.println(c, encoding);
}

void Console::println(int i, int encoding)
{
	Serial.println(i, encoding);
}

void Console::println(unsigned int i, int encoding)
{
	Serial.println(i, encoding);
}

void Console::println(long l, int encoding)
{
	Serial.println(l, encoding);
}

void Console::println(unsigned long l, int encoding)
{
	Serial.println(l, encoding);
}

void Console::println(double d, int precision)
{
	Serial.println(d, precision);
}

void Console::println()
{
	Serial.println();
}

#endif
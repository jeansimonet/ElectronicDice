// DiceCommands.h

#ifndef _DICECONSOLE_h
#define _DICECONSOLE_h

#if defined(_CONSOLE)
#include "Arduino.h"

namespace Systems
{
	#define MAX_WORDS 4

	/// <summary>
	/// Serial port console, used to debug and configure a die
	/// </summary>
	class Console
	{
	public:
		void begin();
		void print(const char string[]);
		void print(char);
		void print(unsigned char, int = DEC);
		void print(int, int = DEC);
		void print(unsigned int, int = DEC);
		void print(long, int = DEC);
		void print(unsigned long, int = DEC);
		void print(double, int = 2);
		void print(const Printable&);

		void println(const __FlashStringHelper *);
		void println(const String &s);
		void println(const char[]);
		void println(char);
		void println(unsigned char, int = DEC);
		void println(int, int = DEC);
		void println(unsigned int, int = DEC);
		void println(long, int = DEC);
		void println(unsigned long, int = DEC);
		void println(double, int = 2);
		void println(const Printable&);
		void println(void);

		int available();
		size_t readBytesUntil(char terminator, char *buffer, size_t length);
		void processCommand(char* data, int len);
	};

	extern Console console;
}

#endif
#endif
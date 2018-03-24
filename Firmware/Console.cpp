#include "Console.h"

#if defined(_CONSOLE)

using namespace Systems;

Console Systems::console;

/// <summary>
/// Initialize the console
/// </summary>
void Console::begin()
{
	override_uart_limit = true;
	Serial.begin(57600);
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
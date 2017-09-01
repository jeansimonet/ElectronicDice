// 
// 
// 

#include "DiceDebug.h"

DiceDebug diceDebug;

DiceDebug::DiceDebug()
	: displayDebug(false)
{
}

void DiceDebug::begin()
{
	override_uart_limit = true;
	Serial.begin(19200);
	Serial.println("Dice Serial COM on");
	displayDebug = true;
}

void DiceDebug::stop()
{
	Serial.println("Dice Serial COM off");
	Serial.end();
	displayDebug = false;
}

bool DiceDebug::isDebugOn()
{
	return displayDebug;
}

void DiceDebug::print(const char string[])
{
	if (displayDebug)
		Serial.print(string);
}

void DiceDebug::print(char c)
{
	if (displayDebug)
		Serial.print(c);
}

void DiceDebug::print(unsigned char c, int encoding)
{
	if (displayDebug)
		Serial.print(c, encoding);
}

void DiceDebug::print(int i, int encoding)
{
	if (displayDebug)
		Serial.print(i, encoding);
}

void DiceDebug::print(unsigned int i, int encoding)
{
	if (displayDebug)
		Serial.print(i, encoding);
}

void DiceDebug::print(long l, int encoding)
{
	if (displayDebug)
		Serial.print(l, encoding);
}

void DiceDebug::print(unsigned long l, int encoding)
{
	if (displayDebug)
		Serial.print(l, encoding);
}

void DiceDebug::print(double d, int precision)
{
	if (displayDebug)
		Serial.print(d, precision);
}

void DiceDebug::println(const char string[])
{
	if (displayDebug)
		Serial.println(string);
}

void DiceDebug::println(char c)
{
	if (displayDebug)
		Serial.println(c);
}

void DiceDebug::println(unsigned char c, int encoding)
{
	if (displayDebug)
		Serial.println(c, encoding);
}

void DiceDebug::println(int i, int encoding)
{
	if (displayDebug)
		Serial.println(i, encoding);
}

void DiceDebug::println(unsigned int i, int encoding)
{
	if (displayDebug)
		Serial.println(i, encoding);
}

void DiceDebug::println(long l, int encoding)
{
	if (displayDebug)
		Serial.println(l, encoding);
}

void DiceDebug::println(unsigned long l, int encoding)
{
	if (displayDebug)
		Serial.println(l, encoding);
}

void DiceDebug::println(double d, int precision)
{
	if (displayDebug)
		Serial.println(d, precision);
}

void DiceDebug::println()
{
	if (displayDebug)
		Serial.println();
}

// 
// Wire library wrapper for the Dice
// 

#include "DiceWire.h"

// We need access to the Wire library, of course!
#include <Wire.h> 

#define SCLpin 21
#define SDApin 24

DiceTWI diceWire;

DiceTWI::AutoLock::AutoLock()
{
	diceWire.lock();
}

DiceTWI::AutoLock::~AutoLock()
{
	diceWire.unlock();
}

void DiceTWI::begin()
{
	Wire.speed = 400;
	Wire.beginOnPins(SCLpin, SDApin);
	busyCount = 0;
}

void DiceTWI::beginTransmission(uint8_t a)
{
	Wire.beginTransmission(a);
}

void DiceTWI::beginTransmission(int a)
{
	Wire.beginTransmission(a);
}

void DiceTWI::end()
{
	Wire.end();
}

void DiceTWI::lock()
{
	// lock the counter, check it AFTER we've locked it
	while (busyCount++ == 0)
	{
		// it wasn't 0, decrement it back
		--busyCount;
	}
	// Otherwise we got it
}

void DiceTWI::unlock()
{
	busyCount--;
}

uint8_t DiceTWI::endTransmission(void)
{
	return Wire.endTransmission();
}

uint8_t DiceTWI::endTransmission(uint8_t a)
{
	return Wire.endTransmission(a);
}

uint8_t DiceTWI::requestFrom(uint8_t a, uint8_t b)
{
	return Wire.requestFrom(a, b);
}

uint8_t DiceTWI::requestFrom(uint8_t a, uint8_t b, uint8_t c)
{
	return Wire.requestFrom(a, b, c);
}

uint8_t DiceTWI::requestFrom(int a, int b)
{
	return Wire.requestFrom(a, b);
}

uint8_t DiceTWI::requestFrom(int a, int b, int c)
{
	return Wire.requestFrom(a, b, c);
}

size_t DiceTWI::write(uint8_t a)
{
	return Wire.write(a);
}

size_t DiceTWI::write(const uint8_t * pa, size_t b)
{
	return Wire.write(pa, b);
}

int DiceTWI::available(void)
{
	return Wire.available();
}

int DiceTWI::read(void)
{
	return Wire.read();
}

int DiceTWI::peek(void)
{
	return Wire.peek();
}

void DiceTWI::flush(void)
{
	Wire.flush();
}

void DiceTWI::onReceive(void(*func)(int))
{
	Wire.onReceive(func);
}

void DiceTWI::onRequest(void(*func)(void))
{
	Wire.onRequest(func);
}

void DiceTWI::onRequestService(void)
{
	Wire.onRequestService();
}

void DiceTWI::onReceiveService(uint8_t* a, int b)
{
	Wire.onReceiveService(a, b);
}


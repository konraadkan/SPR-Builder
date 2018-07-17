#pragma once
#include "HPTimer.h"

class inputs
{
private:
	HPTimer* timer;
public:
	inputs() { timer = new HPTimer; }
	~inputs() { delete timer; }
public:
	bool getKeyStateImmediate(DWORD vKey);
	bool getKeyStateDelay(DWORD vKey, float delay = 0.10f);
};
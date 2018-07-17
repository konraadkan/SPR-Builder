#include "inputs.h"

bool inputs::getKeyStateImmediate(DWORD vKey)
{
	if (GetKeyState(vKey) < 0)
		return true;
	return false;
}

bool inputs::getKeyStateDelay(DWORD vKey, float delay)
{
	timer->Update();
	if (timer->GetTimeTotal() > delay)
	{
		if (GetKeyState(vKey) < 0)
		{
			timer->Reset();
			return true;
		}
	}
	return false;
}
#pragma once
#include <Windows.h>

class HPTimer
{
	long long startTime;				//zero time
	long long lastCallToUpdate;			//previous call
	long long currentCallToUpdate;		//current call
	long long frequency;
public:
	HPTimer()
	{
		LARGE_INTEGER t;
		QueryPerformanceFrequency(&t);
		frequency = t.QuadPart;

		Reset();
	}

	void Reset()
	{
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		startTime = t.QuadPart;
		currentCallToUpdate = t.QuadPart;
		lastCallToUpdate = t.QuadPart;
	}

	void Update()
	{
		lastCallToUpdate = currentCallToUpdate;
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		currentCallToUpdate = t.QuadPart;
	}

	double GetTimeTotal()
	{//seconds since Reset was called
		double d = (double)(currentCallToUpdate - startTime);
		return d / frequency;
	}

	double GetTimeDelta()
	{
		double d = (double)(currentCallToUpdate - lastCallToUpdate);
		return d / frequency;
	}
};
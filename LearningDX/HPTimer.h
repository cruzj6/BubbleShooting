#pragma once
#include <Windows.h>

class HPTimer
{
	long long startTime;
	long long lastCallToUpdate;
	long long currentCallToUpdate;
	long long frequency;

public:
	HPTimer()
	{
		LARGE_INTEGER t;

		//First get the frequency at which the cycles are happening
		QueryPerformanceFrequency(&t);
		frequency = t.QuadPart;

		Reset();
	}

	void Reset()
	{
		LARGE_INTEGER t;
		//Get the amount of cycles in a undetermined interval
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
	{
		//Get the amount of cycles passed since the instantiation of this
		double d = currentCallToUpdate - startTime;
		//Divide that by the frequency to see how many seconds passed
		return d / frequency;
	}

	double GetTimeDelta()
	{
		//Same as total time except for each frame
		double d = currentCallToUpdate - lastCallToUpdate;
		return d / frequency;
	}
};
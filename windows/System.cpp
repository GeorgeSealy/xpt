#include "xpt.h"

#ifndef WINDOWS

#error unsupported platform!

#endif

#include <windows.h>


static bool timer_initialized = false;
static LARGE_INTEGER timer_frequency;
static LARGE_INTEGER timer_start;

double systemTimeInSeconds() {

	if (!timer_initialized) {
		QueryPerformanceFrequency(&timer_frequency);
		QueryPerformanceCounter(&timer_start);
		timer_initialized = true;
	}

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return double(now.QuadPart - timer_start.QuadPart) / double(timer_frequency.QuadPart);
}

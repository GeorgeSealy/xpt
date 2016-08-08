#include "xpt.h"

#ifndef APPLE

#error unsupported platform!

#endif

#include <mach/mach_time.h>

static double ticksToSeconds;
static uint64_t startTick = 0;

double systemTimeInSeconds() {
    
	if (startTick == 0) {
		mach_timebase_info_data_t timebase_info;
		mach_timebase_info(&timebase_info);
		
		ticksToSeconds = ((double)timebase_info.denom / (double)timebase_info.numer) / 1000000000.0;
		
		startTick = mach_absolute_time();
	}
	
	uint64_t nowTick = mach_absolute_time();
	return (nowTick - startTick) * ticksToSeconds;

	// if (!timer_initialized) {
	// 	QueryPerformanceFrequency(&timer_frequency);
	// 	QueryPerformanceCounter(&timer_start);
	// 	timer_initialized = true;
	// }

	// LARGE_INTEGER now;
	// QueryPerformanceCounter(&now);
	// return double(now.QuadPart - timer_start.QuadPart) / double(timer_frequency.QuadPart);
	// return 0.0;

}

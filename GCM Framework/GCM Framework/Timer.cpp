#include "Timer.h"

Timer::Timer() {
	startTime = sys_time_get_system_time();
	lastTime  = GetMS();
};

system_time_t Timer::GetTime() {
	return sys_time_get_system_time()-startTime;
};

double Timer::GetMS() {
	return GetTime()/((double)sys_time_get_timebase_frequency()/100000.0);
}

double Timer::GetMicro() {
	return GetTime()/((double)sys_time_get_timebase_frequency()/1000.0);
}

double Timer::GetTimedMS() {
	double a	= GetMS();
	double b	= a-lastTime;
	lastTime	= a;
	return b;
}
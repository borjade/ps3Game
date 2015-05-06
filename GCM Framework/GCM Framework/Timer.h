/******************************************************************************
Class:Timer
Implements:
Author:Rich Davison
Description: It's the timer class from Graphics for Games...but for the PS3!

Uses the internal PS3 system functions to determine time, rather than going
through the performance counter.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sys/sys_time.h>

class Timer	{
public:
	Timer();

	system_time_t GetTime();

	double GetMS();		//Get milliseconds since timer started
	double GetMicro();  //Get microseconds since timer started
	double GetTimedMS();//get milliseconds since last GetTimedMS call

protected:
	system_time_t startTime;
	double lastTime;
};

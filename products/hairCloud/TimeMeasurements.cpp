/*
 *  TimeMeasurements.cpp
 *  ScanRegistrator
 *
 *  Created by Hao Li on 08.12.07.
 *  Copyright 2007 ETH Zurich - Applied Geometry Group. All rights reserved.
 *
 */

#include "TimeMeasurements.h"
#include <math.h>

namespace systemService{
	
double startEventTime;
	
double getCurrentTime()
{
	double time=0.0;
	#ifdef __APPLE_CC__
	time = CFAbsoluteTimeGetCurrent();
	#endif
	return time;
}
	
void startMeasureTime()
{
	startEventTime = getCurrentTime();
}
	
double ellapsedTime()
{
	return getCurrentTime()-startEventTime;
}
	
std::string printEllapsedTime(double _seconds)
{
	std::string timeString;
	char buffer[100];
	unsigned int seconds = (unsigned int) _seconds;
	unsigned int hours = seconds / 3600;
	unsigned int remainingSecondsOfHours = seconds % 3600;
	unsigned int minutes = remainingSecondsOfHours / 60.0f;
	seconds = remainingSecondsOfHours % 60;
	
	unsigned int milliseconds = (unsigned int)((_seconds - floor(_seconds)) * 1000.0);
	if(hours>0){
		sprintf(buffer,"%d",hours);
		timeString.append(buffer);
		timeString.append(std::string(" hours "));
	}
	if(minutes>0)
	{
		sprintf(buffer,"%d",minutes);
		timeString.append(buffer);
		timeString.append(std::string(" minutes "));
	}
	if(seconds>0)
	{
		sprintf(buffer,"%d",seconds);
		timeString.append(buffer);
		timeString.append(std::string(" seconds "));
	}
	sprintf(buffer,"%d",milliseconds);
	timeString.append(buffer);
	timeString.append(std::string(" milliseconds"));
	
	return timeString;
}
}
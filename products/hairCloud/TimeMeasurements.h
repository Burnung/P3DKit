/*
 *  TimeMeasurements.h
 *  ScanRegistrator
 *
 *  Created by Hao Li on 23.03.07.
 *  Copyright 2007 ETH Zurich - Applied Geometry Group. All rights reserved.
 *
 */

#ifndef TIME_H
#define TIME_H

#include <string>

#ifdef __APPLE_CC__	
	#include <CoreFoundation/CoreFoundation.h>
#endif


namespace systemService{

double getCurrentTime();

void startMeasureTime();

double ellapsedTime();

std::string printEllapsedTime(double _seconds);
}
#endif
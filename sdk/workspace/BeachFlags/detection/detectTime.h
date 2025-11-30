#ifndef DETECT_TIME_H_
#define DETECT_TIME_H_

#include "module_common.h"
#include "detect.h"
#include <kernel.h>

class DetectTime : public Detect
{
public:
	DetectTime(int interval);
	bool detect() override;

private:
	int mInterval;
	SYSTIM startTime;
	SYSTIM now;
	SYSTIM elapsedTime;
	bool isStartTimeObtained;
};

#endif // DETECT_TIME_H_
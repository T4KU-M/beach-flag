#ifndef DETECT_TIME_H_
#define DETECT_TIME_H_

#include "module_common.h"
#include "detect.h"
#include <kernel.h>

class DetectTimeOrHSV : public Detect
{
public:
	DetectTimeOrHSV(int interval);
	bool detect() override;

private:
	int mInterval;
	SYSTIM startTime;
	SYSTIM now;
	SYSTIM elapsed;
};

#endif // DETECT_TIME_H_
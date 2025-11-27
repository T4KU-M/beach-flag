#ifndef DETECT_ANGLE_FOR_CURRENT_TARGET_VALUE_H_
#define DETECT_ANGLE_FOR_CURRENT_TARGET_VALUE_H_

#include "module_common.h"
#include "detect.h"
//#include "localizer.h"
#include "calculateAngle.h"

class DetectAngleForCurrentTargetValue : public Detect
{
public:
	DetectAngleForCurrentTargetValue();
	bool detect() override;

private:
	CalculateAngle mCalculateAngle;
	double mTargetAngle; // 単位：度
};

#endif

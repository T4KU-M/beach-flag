
#ifndef DETECT_ANGLE_H_
#define DETECT_ANGLE_H_

#include "module_common.h"
#include "detect.h"
//#include "localizer.h"
#include "calculateAngle.h"

class DetectAngle : public Detect
{
public:
	DetectAngle(double minAngle);
	bool detect() override;

private:
	CalculateAngle mCalculateAngle;
	double mMinAngle;
};

#endif //xddw

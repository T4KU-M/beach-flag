
#ifndef DETECT_ANGLEFORPIC_H_
#define DETECT_ANGLEFORPIC_H_

#include "module_common.h"
#include "detect.h"
//#include "localizer.h"
#include "calculateAngle.h"

class DetectAngleforpic : public Detect
{
public:
	DetectAngleforpic(Target target);
	bool detect() override;

private:
	CalculateAngle mCalculateAngle;
	Target mTarget;
};

#endif //xddw

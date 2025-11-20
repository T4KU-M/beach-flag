#ifndef DETECT_MOTORANGLE_H_
#define DETECT_MOTORANGLE_H_

#include "module_common.h"
//#include <Motor.h>
#include <motor.h>
#include "detect.h"


class DetectMotorAngle : public Detect
{
public:
	DetectMotorAngle(double minAngle, double maxAngle);
	bool detect() override;

private:
	void resetEncoders();
	
	double mMinAngle, mMaxAngle, mAngle;
	bool mReset;
};

#endif //DETECT_MOTORANGLE_H_
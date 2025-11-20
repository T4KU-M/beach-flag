#ifndef DETECT_SONAR_H_
#define DETECT_SONAR_H_

#include "module_common.h"
#include "detect.h"
#include <ultrasonicsensor.h>

class DetectSonar : public Detect
{
public:
	DetectSonar(int distanceMax, int countMax);
    bool detect() override;

private:
    int mDistanceMax;
    int mCountMax, mCount;
};

#endif // DETECT_SONAR_H_
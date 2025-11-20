#ifndef DETECT_SONAR_AND_DISTANCE_H_
#define DETECT_SONAR_AND_DISTANCE_H_

#include "module_common.h"
#include "detect.h"
#include "localizer.h"
//#include <SonarSensor.h>

class DetectSonarAndDistance : public Detect
{
public:
	DetectSonarAndDistance(int distanceMax, int countMax, double threTravelDistance);
    bool detect() override;

private:
    int mDistanceMax;
    int mCountMax, mCount;

    Localizer mLocalizer;
	double mThreTravelDistance;
};

#endif // DETECT_SONAR_AND_DISTANCE_H_
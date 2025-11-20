#ifndef DETECT_DISTANCE_H_
#define DETECT_DISTANCE_H_

#include "module_common.h"
#include "detect.h"
#include "localizer.h"

// 距離検知クラス
class DetectDistance : public Detect
{
public:
	DetectDistance(double threTravelDistance);
	bool detect() override;

private:
	Localizer mLocalizer;
	double mThreTravelDistance;
};

#endif

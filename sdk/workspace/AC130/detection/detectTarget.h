#ifndef DETECT_TARGET_H_
#define DETECT_TARGET_H_

#include "module_common.h"
#include "detect.h"
#include "localizer.h"
#include "hsvMeasure.h"

// 距離検知クラス
class DetectTarget : public Detect
{
public:
	DetectTarget();
	bool detect() override;

private:
	Localizer mLocalizer;
	HsvMeasure mHsvMeasure;
	int mCount;
	double mTargetDistance;
};

#endif

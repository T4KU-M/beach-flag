#ifndef DETECT_GARAGECOLOR_H_
#define DETECT_ARAGECOLOR_H_

#include "module_common.h"
#include "detect.h"
#include "hsvMeasure.h"

// ガレージカラー検知クラス
class DetectGarageColor : public Detect
{
public:
	DetectGarageColor();

	bool detect() override;

private:
	HsvMeasure mHsvMeasure;
	int mLoopCount;
	int mCountBlue, mCountYellow, mCountRed;
};

#endif

#ifndef DETECT_BRIGHTNESS_H_
#define DETECT_BRIGHTNESS_H_

#include "module_common.h"
#include "hsvMeasure.h"
#include "detect.h"

class DetectBrightness : public Detect
{
public:
	DetectBrightness(int brightnessMin, int brightnessMax);
	bool detect() override;

private:
	HsvMeasure mHsvMeasure;
	int mMinThreshold, mMaxThreshold;
};

#endif

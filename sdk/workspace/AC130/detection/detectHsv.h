#ifndef DETECT_HSV_H_
#define DETECT_HSV_H_

#include "module_common.h"
#include "detect.h"
#include "hsvMeasure.h"

// ゴールライン検知クラス
class DetectHsv : public Detect
{
public:
	DetectHsv(
		int minH = 0, int maxH = 360,
		int minS = 0, int maxS = 255,
		int minV = 0, int maxV = 500);

	bool detect() override;

	// private:
	HsvMeasure mHsvMeasure;
	int mMinH, mMaxH, mMinS, mMaxS, mMinV, mMaxV;
	int mCount;
};

#endif

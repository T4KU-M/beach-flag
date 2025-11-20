#include "detectBrightness.h"


// コンストラクタ
DetectBrightness::DetectBrightness(int minThreshold, int maxThreshold)
	: mHsvMeasure(),
	  mMinThreshold(minThreshold),
	  mMaxThreshold(maxThreshold)
{
}

// 検知結果を確認する
bool DetectBrightness::detect()
{
	int h, s, v;
	mHsvMeasure.getHSV(h, s, v);

	return (v >= mMinThreshold) && (v < mMaxThreshold);
}

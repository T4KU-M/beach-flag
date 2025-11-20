#include "detectTarget.h"

// コンストラクタ
DetectTarget::DetectTarget()
	: mLocalizer(),
	  mHsvMeasure(),
	  mCount(0),
	  mTargetDistance(0.0)
{
}

// 検知結果を確認する
bool DetectTarget::detect()
{
	mCount++;
	mLocalizer.update();
	int h, s, v;
	mHsvMeasure.getHSV(h, s, v);

	if(mCount == 1){
		mTargetDistance = (targetdistance > 200)? targetdistance:200;
		return false;
	}
	
	printf("%f,%f\n", mLocalizer.travelDistance(),mTargetDistance);

	if(mLocalizer.travelDistance() > mTargetDistance-80){
		return true;
	}

	if ((h > gBlueMinH) && (h < gBlueMaxH) &&
		(s > gBlueMinS) && (s < gBlueMaxS) &&
		(v > gBlueMinV) && (v < gBlueMaxV))
	{
		return true;
	}
	return false;
}
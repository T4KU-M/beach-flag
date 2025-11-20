#include "detectDistance.h"

// コンストラクタ
DetectDistance::DetectDistance(double threTravelDistance)
	: mLocalizer(),
	  mThreTravelDistance(threTravelDistance)
{
}

// 検知結果を確認する
bool DetectDistance::detect()
{
	mLocalizer.update();
	return mLocalizer.travelDistance() > mThreTravelDistance;
}
#include "detectHsv.h"
#include "module_common.h"

// コンストラクタ
DetectHsv::DetectHsv(
	int minH, int maxH,
	int minS, int maxS,
	int minV, int maxV)
	: mHsvMeasure(),
	  mMinH(minH), mMaxH(maxH),
	  mMinS(minS), mMaxS(maxS),
	  mMinV(minV), mMaxV(maxV),
	  mCount(0)
{
}

// 検知結果を確認する
bool DetectHsv::detect()
{
	int h, s, v;
	mHsvMeasure.getHSV(h, s, v);
	printf("hsv %3d %3d %3d : min %3d %3d %3d : max %3d %3d %3d\n", h, s, v,mMinH, mMinS, mMinV,mMaxH, mMaxS, mMaxV);
	//printf("DetectHsv: h=%d, s=%d, v=%d\n", mMinH, mMinS, mMinV);
	//printf("DetectHsv: h=%d, s=%d, v=%d\n", h, s, v);
	if ((h > mMinH) && (h < mMaxH) &&
		(s > mMinS) && (s < mMaxS) &&
		(v > mMinV) && (v < mMaxV))
	{
		mCount=1;
		printf("%d",mCount);
	}
	else
	{
		mCount = 0;
	}
	//printf("%d\n",mCount);
	if(mCount == 1){
		printf("検知\n");
	}
	return (mCount >= 1);
}

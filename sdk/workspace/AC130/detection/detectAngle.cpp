#include "detectAngle.h"


// コンストラクタ
DetectAngle::DetectAngle(double minAngle)
	: mCalculateAngle(),
	  mMinAngle(minAngle)
{
}

// 検知結果を確認するaaaa
bool DetectAngle::detect()
{
	double theta;
	
	mCalculateAngle.update();
	mCalculateAngle.getangle(theta);
	printf("%f\n",theta);
	
	if(0 >= mMinAngle){
		if(mMinAngle >= theta){
			angleSum += theta;
			return true;
		}
		return false;
	}
	else{
		if(mMinAngle <= theta){
			angleSum += theta;
			return true;
		}
		return false;
	}
	
}
//

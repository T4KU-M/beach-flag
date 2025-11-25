#include "detectAngle.h"
#include "module_common.h"


// コンストラクタ
DetectAngle::DetectAngle(double minAngle)
	: mCalculateAngle(),
	  mMinAngle(minAngle)
{
}

// 検知結果を確認する
bool DetectAngle::detect()
{
	double theta;
	
	mCalculateAngle.update();
	mCalculateAngle.getangle(theta);
	printf("角度：%f,%f,%f\n",theta,mMinAngle,gateangle);
	
	if(0 >= mMinAngle){
		if(mMinAngle >= theta){
			return true;
		}
		return false;
	}
	else{
		if(mMinAngle <= theta){
			return true;
		}
		return false;
	}
}
//

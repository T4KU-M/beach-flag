#include "detectAngleForCurrentTargetValue.h"
#include "module_common.h"


// コンストラクタ
DetectAngleForCurrentTargetValue::DetectAngleForCurrentTargetValue()
	: mCalculateAngle(),
{
}

// 検知結果を確認する
bool DetectAngle::detect()
{
    // 現在の角度取得
	double theta; // 単位：度
	mCalculateAngle.update();
	mCalculateAngle.getangle(theta);

    mTargetAngle = currentTargetTheta * 180/3.1415; // radian -> degree
	printf("現在の角度：%f, 目標値：%f\n",theta, mTargetAngle);

    // 目標角度と比較して検知
	
	if(mTargetAngle < 0){
		if(theta <= mTargetAngle){
			return true;
		}
		return false;
	}
	else{
		if(theta >= mTargetAngle){
			return true;
		}
		return false;
	}
}

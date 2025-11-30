#include "detectAngleForCurrentTargetValue.h"
#include "module_common.h"


// コンストラクタ
DetectAngleForCurrentTargetValue::DetectAngleForCurrentTargetValue()
	: mCalculateAngle()
{
}

// 検知結果を確認する
bool DetectAngleForCurrentTargetValue::detect()
{
    // 現在の角度取得
	double theta; // 単位：度
	mCalculateAngle.update();
	mCalculateAngle.getangle(theta);

    mTargetAngle = ((int)(currentTargetTheta * 180/3.1415)) % 360; // radian -> degree
	if(mTargetAngle > 180){
		mTargetAngle -= 360;
	}
	// else if(mTargetAngle < -180){
	// 	mTargetAngle += 360;
	// }
	printf("現在の角度：%f, 目標値：%d\n",theta, mTargetAngle);

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

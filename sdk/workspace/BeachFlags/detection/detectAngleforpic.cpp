#include "detectAngleforpic.h"
#include "module_common.h"


// コンストラクタ
DetectAngleforpic::DetectAngleforpic(Target target)
	: mCalculateAngle(),
	  mTarget(target)
{
}

// 検知結果を確認する
bool DetectAngleforpic::detect()
{
	double theta;
	double Angle = (mTarget == Gate) ? gateangle : targetangle;
	
	mCalculateAngle.update();
	mCalculateAngle.getangle(theta);
	//printf("角度：%f,%f\n",theta,Angle);
	
	if(0 >= Angle){
		if(Angle >= theta -4){
			return true;
		}
		return false;
	}
	else{
		if(Angle <= theta+4){
			return true;
		}
		return false;
	}
}
//

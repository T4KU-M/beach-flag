#include "detectAngleforpic.h"
#include "module_common.h"


// コンストラクタ
DetectAngleforpic::DetectAngleforpic(Target target)
	: mCalculateAngle(),
	  mTarget(target)
{
	if(mTarget != Gate){
		findgate = 0;
	}
}

// 検知結果を確認するaaaa
bool DetectAngleforpic::detect()
{
	double theta;
	double Angle = 0.0;
	mCalculateAngle.update();
	mCalculateAngle.getangle(theta);


	if(mTarget == Gate || mTarget == Marker){
		Angle = (mTarget == Gate) ? gateangle : targetangle;
		printf("角度：%f,%f,%f,%d\n",theta,Angle,gateangle,targetangle,mTarget);
	//marker or gate
		if(findgate==1 && mTarget == Gate){
			findgate=0;
			return true;
		}

		if(Angle != 0.0 ){
			printf("検知できてる\n");
			if(0 >= Angle){
				if(Angle >= theta -3){
					angleSum += theta;
					if(mTarget == Gate){
						findgate = 1;
					}
					return true;
				}
				return false;
			}
			else{
				if(Angle <= theta+3){
					angleSum += theta;
					if(mTarget == Gate){
						findgate = 1;
					}
					return true;
				}
				return false;
			}
		}
		else if(findgate == 0 && mTarget == Gate){
			if(theta >= 15){
				angleSum += theta;
				findgate = 0;
				return true;
			}
			return false;
		}
		else{
			if(theta >= 0){
				angleSum += theta;
				findgate = 0;
				return true;
			}
			return false;
		}	
	}
	else{
		if(mTarget == Mode0){
			Angle = -20 - angleSum;
		}
		else if(mTarget == Mode135){
			if(angleSum > 75){
				Angle = 0;
			}else{
				Angle = 115 - angleSum;
			}
		}
		else if(mTarget == Mode135R){
			Angle = 100 - angleSum;
		}
		else if(mTarget == Mode180){
			Angle = 175 - angleSum;
		}
		if(0 >= Angle){
			if(Angle >= theta -4){
				angleSum += theta;
				return true;
			}
			return false;
		}
		else{
			if(Angle <= theta+4){
				angleSum += theta;
				return true;
			}
			return false;
		}

	}
}
//

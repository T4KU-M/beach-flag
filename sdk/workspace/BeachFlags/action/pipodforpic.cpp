//#include "Motor.h"
#include <motor.h>
#include "pipodforpic.h"
#include "module_common.h"

// コンストラクタ
Pipodforpic::Pipodforpic(Target target)
	:mTarget(target), mReset(true)
{
}

// 旋回量を決定する(override)
void Pipodforpic::determineSpeedAndSteering()
{
		if (mReset)
		{
			pup_motor_reset_count(gRobot.leftMotor());
			pup_motor_reset_count(gRobot.rightMotor());
			mReset = false;
		}
		double angle = (mTarget == Gate) ? gateangle : targetangle;
		int u = 80;
		int s = 1;
		s = (angle >= 0)? 1 : -1; 
		setSpeed(u);		// 速度を設定
		setSteering((u + 10)*s);		// 旋回量を設定

}

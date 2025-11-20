//#include "Motor.h"
#include <motor.h>
#include "pipod.h"
#include "module_common.h"

// コンストラクタ
Pipod::Pipod(LeftOrRight LineEdge)
	:mLineEdge(LineEdge), mReset(true)
{
}

// 旋回量を決定する(override)
void Pipod::determineSpeedAndSteering()
{
		if (mReset)
		{
			pup_motor_reset_count(gRobot.leftMotor());
			pup_motor_reset_count(gRobot.rightMotor());
			mReset = false;
		}
		int u = 80;
		int s = 1;
		s = (mLineEdge == Left)? 1 : -1; 
		setSpeed(u);		// 速度をゼロにする
		setSteering((u + 10)*s);		// 旋回量をゼロにする

}

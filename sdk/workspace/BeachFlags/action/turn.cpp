#include "turn.h"
//#include "Motor.h"
#include <motor.h>
//#include "MotorPair.h"

// コンストラクタ
Turn::Turn(int fixedTurningAmount, int speedMin, int speedMax, int kp)
	: Run(speedMin, speedMax, kp), mFixedTurningAmount(fixedTurningAmount), mSpeedMin(speedMin), kp(kp), mReset(true)
{
	printf("Turn::Turn()\n");
}

// 旋回量を決定する(override)
void Turn::determineSteering() // 左右のモータのエンコーダのずれを使ってP制御で旋回量を決定
{
	if(mFixedTurningAmount==0 && mSpeedMin!=0)
	{
		if (mReset)
		{
			pup_motor_reset_count(gRobot.leftMotor());
			pup_motor_reset_count(gRobot.rightMotor());
			mReset = false;
		}

		double u = 0.0;
		const double kp_counst = 1.0;
		int plusMinus = -1;
		if(mSpeedMin<0){plusMinus = 1;}

		//detectMotorAngleへ変える
		double deltAngle = 0.0;
		//deltAngle = (gRobot.rightMotor()->getCount() - gRobot.leftMotor()->getCount());
		double rightCount = pup_motor_get_count(gRobot.rightMotor());
		double leftCount  = pup_motor_get_count(gRobot.leftMotor());
		deltAngle  = rightCount - leftCount;
		
		// P制御（比例）
		// u = plusMinus * kp * deltAngle;
		u = plusMinus * kp_counst * deltAngle;

		int mFixedTurningAmount = (int)u;
		mFixedTurningAmount = (mFixedTurningAmount > 10) ? 10 : mFixedTurningAmount;
		mFixedTurningAmount = (mFixedTurningAmount < -10) ? -10 : mFixedTurningAmount;


		setSteering(mFixedTurningAmount);
		printf("Turn::determineSteering()\n");
	}
	else
	{
		setSteering(mFixedTurningAmount);
	}

}
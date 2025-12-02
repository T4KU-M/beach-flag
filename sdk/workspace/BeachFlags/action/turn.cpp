#include "turn.h"
//#include "Motor.h"
#include <motor.h>
//#include "MotorPair.h"

// コンストラクタ
Turn::Turn(int fixedTurningAmount, int speedMin, int speedMax, double kp)
	: Run(speedMin, speedMax), mFixedTurningAmount(fixedTurningAmount), mSpeedMin(speedMin), kp(kp), mReset(true), cycle_count(0)
{
	printf("Turn::Turn()\n");
}

// 旋回量を決定する(override)
void Turn::determineSteering()
{
	this->cycle_count = this->cycle_count + 1; // カウントアップ
	if(mFixedTurningAmount==0 && mSpeedMin!=0)
	{
		if (mReset)
		{
			pup_motor_reset_count(gRobot.leftMotor());
			pup_motor_reset_count(gRobot.rightMotor());
			mReset = false;
		}

		double u = 0.0;
		// double kp = 1.0;
		int plusMinus = -1;
		if(mSpeedMin<0){plusMinus = 1;}

		//detectMotorAngleへ変える
		double deltAngle = 0.0;
		//deltAngle = (gRobot.rightMotor()->getCount() - gRobot.leftMotor()->getCount());
		double rightCount = pup_motor_get_count(gRobot.rightMotor());
		double leftCount  = pup_motor_get_count(gRobot.leftMotor());
		deltAngle  = rightCount - leftCount;
		
		// P制御（比例）
		u = plusMinus * kp * deltAngle;

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

void Turn::setSpeed(int value)
{
	this->cycle_count = this->cycle_count + 1; // カウントアップ
	kp = int(this->kp * 5); // 元に戻すよーん
	// 値を -100 ～ 100 の範囲に制限
	value = (value < -100) ? -100 : ((value > 100) ? 100 : value);

    // valueを-100~-45, 45~100の範囲に変換
    if (value < 0){
        value = -100 + ( (value + 100) * (55.0 / 100.0) );  // 55 = -45 - (-100)
    }else{
        value = 45 + ( value * (55.0 / 100.0) );            // 55 = 100 - 45
    }
	if (this->cycle_count % 20 == 0) {
		if (value < 0){
			kp = kp * -1;
		}
		value = value + kp;
		printf("add acceleration kp: %f\n", kp);
	}
	mSpeed = value;
}
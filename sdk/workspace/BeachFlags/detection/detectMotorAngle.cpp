#include "detectMotorAngle.h"

// コンストラクタ
DetectMotorAngle::DetectMotorAngle(double minAngle, double maxAngle)
	:mMinAngle(minAngle), mMaxAngle(maxAngle), mAngle(), mReset(true)
{
}


// 検知結果を確認する
bool DetectMotorAngle::detect()
{
	if (mReset)
	{
		resetEncoders(); //初回だけモータ回転角をリセット
	}
		

	double thetaLeft, thetaRight;
	//thetaLeft = gRobot.leftMotor()->getCount();
	thetaLeft = pup_motor_get_count(gRobot.leftMotor());
	thetaRight = pup_motor_get_count(gRobot.rightMotor());
    mAngle = abs(thetaLeft - thetaRight);
	//mAngle = thetaRight - thetaLeft;
	//printf("mAngle=%f\n",mAngle);

	if((mAngle >= mMinAngle) && (mAngle < mMaxAngle))
	{
		//printf("********************************\n");
		return true;
	}
    else
	{
		return false;
	}
}

// モータ回転角をリセットする
void DetectMotorAngle::resetEncoders()
{
	//gRobot.leftMotor()->setCount(0);
	pup_motor_reset_count(gRobot.leftMotor());
	//gRobot.rightMotor()->setCount(0);
	pup_motor_reset_count(gRobot.rightMotor());
	mReset = false;
}

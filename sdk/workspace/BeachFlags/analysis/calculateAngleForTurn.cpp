#include "calculateAngleForTurn.h"
#include <spike/hub/imu.h>
#include "module_common.h"

// コンストラクタ
CalculateAngleForTurn::CalculateAngleForTurn()
	: mTheta(0), 
	  mReset(true)
{
}

// 自己位置を更新する
void CalculateAngleForTurn::update()
{
	if (mReset)
	{
		hub_imu_init();	
		// mReset = false;
	}

	// 回転角を読み取る

	double dTheta;
	float angular_velocity[3] = {0};

	// 自己位置を計算
	hub_imu_get_angular_velocity(angular_velocity);
	dTheta = (angular_velocity[0]*cos(39*3.1415/180)-angular_velocity[1]*sin(39*3.1415/180))/1000*16.75;  //833回取得してるらしい
	mTheta += dTheta;

	if (mReset)
	{
		// ビーチフラッグ用改造：初期角度を-180度とする
		mTheta = -180;	
		mReset = false;
	}
}

// 自己位置と進行方向を取得する
void CalculateAngleForTurn::getangle(double &theta) const
{
	theta = mTheta;
}
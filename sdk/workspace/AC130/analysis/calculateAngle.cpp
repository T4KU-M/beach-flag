#include "calculateAngle.h"
#include <spike/hub/imu.h>
#include <math.h>

// コンストラクタ
CalculateAngle::CalculateAngle()
	: mTheta(0), 
	  mReset(true)
{
}

// 自己位置を更新するaa
void CalculateAngle::update()
{
	if (mReset)
	{
		hub_imu_init();	
		mReset = false;
	}

	// 回転角を読み取る

	double dTheta;
	float angular_velocity[3] = {0};

	// 自己位置を計算
	hub_imu_get_angular_velocity(angular_velocity);
	dTheta = (angular_velocity[0]*cos(39*3.1415/180)-angular_velocity[1]*sin(39*3.1415/180))/1000*16.75;  //833回取得してるらしい
	mTheta += dTheta;
}

// 自己位置と進行方向を取得する
void CalculateAngle::getangle(double &theta) const
{
	theta = mTheta;
}
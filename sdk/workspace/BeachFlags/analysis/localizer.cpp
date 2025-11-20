#include "localizer.h"

// sinc関数
static double sinc(double x)
{
	double ans = 0, coef[5] = { 1, 1.666667e-01, 8.333333e-03, 1.984127e-04, 2.755732e-06 };
	for (int i = 0; i < 5; i++) { ans += coef[i] * std::pow(x, 2 * i); }
	return ans;
}

// コンストラクタ
Localizer::Localizer()
	: mX(), mY(), mTheta(), mTravelDistance(0), mLeftEncoder(), mRightEncoder(),
	  mReset(true)
{
}

// 自己位置を更新する
void Localizer::update()
{
	if (mReset)
	{
		pup_motor_reset_count(gRobot.leftMotor());
		pup_motor_reset_count(gRobot.rightMotor());
		mReset = false;
	}

	// 回転角を読み取る
	const int curr = 0, prev = 1;
	mLeftEncoder[prev] = mLeftEncoder[curr];
	mLeftEncoder[curr] = pup_motor_get_count(gRobot.leftMotor());
	mRightEncoder[prev] = mRightEncoder[curr];
	mRightEncoder[curr] = pup_motor_get_count(gRobot.rightMotor());

	const double WheelDiameter = 56, WheelTrackWidth = 140;
	double L, R, dX, dY, dTheta;

	// 自己位置を計算
	L = (WheelDiameter / 2) * (mLeftEncoder[curr] - mLeftEncoder[prev]) * pi / 180;
	R = (WheelDiameter / 2) * (mRightEncoder[curr] - mRightEncoder[prev]) * pi / 180;
	dTheta = (R - L) / WheelTrackWidth;
	dX = (R + L) / 2 * std::cos(mTheta + dTheta / 2) * sinc(dTheta / 2);
	dY = (R + L) / 2 * std::sin(mTheta + dTheta / 2) * sinc(dTheta / 2);

	mX += dX; mY += dY; mTheta += dTheta;
	mTravelDistance += std::sqrt(std::pow(dX, 2) + std::pow(dY, 2));
}

// 自己位置と進行方向を取得する
void Localizer::coordinates(double &x, double &y, double &theta) const
{
	x = mX; y = mY; theta = mTheta;
}

// 走行距離を取得する
double Localizer::travelDistance() const
{
	return mTravelDistance;
}

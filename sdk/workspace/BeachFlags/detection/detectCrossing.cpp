#include "detectCrossing.h"

// コンストラクタ
DetectCrossing::DetectCrossing()
	: mState(0)
{
}

// 検知結果を確認する
bool DetectCrossing::detect()
{
	pup_color_rgb_t rgb = pup_color_sensor_rgb(gRobot.colorSensor());

	switch (mState)
	{
	case 0:
		if (rgb.r < 20) { mState = 1; }
		return false;

	case 1:
		return (rgb.r >= 50);
	}
}

#include "hsvMeasure.h"


// コンストラクタ
HsvMeasure::HsvMeasure()
	: mH(0), mS(0), mV(0)
{
}



//HSV値を取得する
void HsvMeasure::getHSV(int &h, int &s, int &v)
{
	ConvertRGBtoHSV();
	h=mH;
	s=mS;
	v=mV;
}

//RGB値をHSV値に変換する
void HsvMeasure::ConvertRGBtoHSV()
{
	//rgb_raw_t rgb;
	//pup_color_rgb_t rgb;

	//gRobot.colorSensor()->getRawColor(rgb); // RGB値取得
	pup_color_rgb_t rgb = pup_color_sensor_rgb(gRobot.colorSensor());

	uint16_t max, min;

	// R, G, Bの値で最大のものを求める
	auto Max = [](uint16_t x, uint16_t y) { return (x > y) ? x : y; };
	auto Min = [](uint16_t x, uint16_t y) { return (x < y) ? x : y; };

	max = Max(rgb.r, Max(rgb.g, rgb.b));
	min = Min(rgb.r, Min(rgb.g, rgb.b));

	// Hを求める
	if (max == min)
	{
		mH = 0;
	}
	else if (max == rgb.r)
	{
		mH = (int)(60.0 * ((double)(rgb.g - rgb.b) / (double)(max - min)));
	}
	else if (max == rgb.g)
	{
		mH = (int)(60.0 * ((double)(rgb.b - rgb.r) / (double)(max - min))) + 120;
	}
	else
	{
		mH = (int)(60.0 * ((double)(rgb.r - rgb.g) / (double)(max - min))) + 240;
	}

	if (mH < 0)
	{						 // 0より小さい場合は
		mH += 360; // 360を加えて調整する
	}

	// Sを求める
	mS = (int)(255.0 * ((double)(max - min) / (double)max));

	// Vを求める
	mV = max;

}

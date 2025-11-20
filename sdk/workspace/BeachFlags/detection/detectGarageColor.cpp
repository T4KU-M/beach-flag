#include "detectGarageColor.h"

//青色
#define BLUE_H_MIN       191
#define BLUE_H_MAX      (BLUE_H_MIN + 40)
#define BLUE_S_MIN      0
#define BLUE_S_MAX      255
#define BLUE_V_MIN      0
#define BLUE_V_MAX      255

//黄色
#define YELLOW_H_MIN     24
#define YELLOW_H_MAX    (YELLOW_H_MIN + 40)
#define YELLOW_S_MIN    0
#define YELLOW_S_MAX    255
#define YELLOW_V_MIN    0
#define YELLOW_V_MAX    255

//赤色
#define RED_H_MIN     332
#define RED_H_MAX    (RED_H_MIN + 40)
#define RED_S_MIN    0
#define RED_S_MAX    255
#define RED_V_MIN    0
#define RED_V_MAX    255

//緑色
#define GREEN_H_MIN     137
#define GREEN_H_MAX    (GREEN_H_MIN + 40)
#define GREEN_S_MIN    0
#define GREEN_S_MAX    255
#define GREEN_V_MIN    0
#define GREEN_V_MAX    255

#define GARAGE_RED          0
#define GARAGE_GREEN        1
#define GARAGE_YELLOW       2
#define GARAGE_BLUE         3




// コンストラクタ
DetectGarageColor::DetectGarageColor()
	: mHsvMeasure(),
	  mLoopCount(0),
	  mCountBlue(0), mCountYellow(0),mCountRed(0)
{
	//ガレージの色のデフォルトを緑とする
	gGarageColor=GARAGE_GREEN;
}


// 検知結果を確認する
bool DetectGarageColor::detect()
{
	int h, s, v;
	mHsvMeasure.getHSV(h, s, v);

    //青色の場合
	if ((h > BLUE_H_MIN) && (h < BLUE_H_MAX) &&
		(s > BLUE_S_MIN) && (s < BLUE_S_MAX) &&
		(v > BLUE_V_MIN) && (v < BLUE_V_MAX))
	{
		mCountBlue++;
		if(mCountBlue >= 3){
			gGarageColor=GARAGE_BLUE;
		}
	}
	//黄色の場合
	else if((h > YELLOW_H_MIN) && (h < YELLOW_H_MAX) &&
		(s > YELLOW_S_MIN) && (s < YELLOW_S_MAX) &&
		(v > YELLOW_V_MIN) && (v < YELLOW_V_MAX))
	{
		mCountYellow++;
		if(mCountYellow >= 3){
			gGarageColor=GARAGE_YELLOW;
		}
	}
	//赤色の場合
	else if((h > RED_H_MIN) && (h < RED_H_MAX) &&
		(s > RED_S_MIN) && (s < RED_S_MAX) &&
		(v > RED_V_MIN) && (v < RED_V_MAX))
	{
		mCountRed++;
		if(mCountRed >= 3){
			gGarageColor=GARAGE_RED;
		}
	}
	//その他の場合は、コンストラクタで指定した緑となる
	else
	{
	}

	//100回のループ（１秒）で検知を終了する
    if(mLoopCount >= 100)
	{
		printf("0=赤, 1=緑, 2=黄色, 3=青\n");
		printf("ガレージは%dです\n",gGarageColor);
		return true;
	}

	mLoopCount++;
	return false;

}

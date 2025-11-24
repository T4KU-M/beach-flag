#ifndef MODULE_COMMON_H_
#define MODULE_COMMON_H_

// 標準ライブラリはEV3APIよりも前にincludeする必要あり
#include <cmath>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
// #include "ev3api.h"
#include "spikeapi.h"
#include "robot.h"

enum LeftOrRight
{
    Left = 0,
    Right
};
enum Target
{
    Gate = 0,
    Marker,
    Mode0,
    Mode180,
    Mode135,
    Mode135R
};
const double pi = 3.14159265358979323846;

// グローバル変数
extern int gBlueMinH, gBlueMaxH;
extern int gBlueMinS, gBlueMaxS;
extern int gBlueMinV, gBlueMaxV;
extern int gBlack, gWhite;
extern bool gCheck;
extern int gSlaNo;
extern int gGarageColor;
extern Robot gRobot;
extern double gateangle;
extern double gatedistance;
extern double targetangle;
extern double targetdistance;
extern double angleSum;
extern int findgate;

// ビーチフラッグ用に追加
extern int gRedMinH, gRedMaxH;
extern int gRedMinS, gRedeMaxS;
extern int gRedMinV, gRedMaxV;
extern int gGreenMinH, gGreenMaxH;
extern int gGreenMinS, gGreenMaxS;
extern int gGreenMinV, gGreenMaxV;

#endif

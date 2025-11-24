#include "module_common.h"

// グローバル変数の実体
int gBlueMinH, gBlueMaxH;
int gBlueMinS, gBlueMaxS;
int gBlueMinV, gBlueMaxV;
int gBlack, gWhite;
bool gCheck;
int gSlaNo;
int gGarageColor;
Robot gRobot;

// ビーチフラッグ用に追加
int gRedMinH, gRedMaxH;
int gRedMinS, gRedMaxS;
int gRedMinV, gRedMaxV;

int gGreenMinH, gGreenMaxH;
int gGreenMinS, gGreenMaxS;
int gGreenMinV, gGreenMaxV;

double gateangle ;
double gatedistance;
double targetangle;
double targetdistance;
double angleSum = 0; // 角度の合計
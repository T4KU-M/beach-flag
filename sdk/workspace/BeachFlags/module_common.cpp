#include "module_common.h"

// グローバル変数の実体
HSVThreshold BLUE;
HSVThreshold RED;
int gBlack, gWhite;
bool gCheck;
int gSlaNo;
int gGarageColor;
Robot gRobot;

// ビーチフラッグ用に追加
int gRedMinH, gRedMaxH;
int gRedMinS, gRedMaxS;
int gRedMinV, gRedMaxV;
double currentX, currentY, currentTheta;
double currentTargetTheta; // 可変量の旋回検知に対応するための変数


double gateangle ;
double gatedistance;
double targetangle;
double targetdistance;
double angleSum = 0; // 角度の合計
#include "module_common.h"

// グローバル変数の実体
HSVThreshold BLUE;
HSVThreshold RED;
int gBlack, gWhite;
bool gCheck;
int gSlaNo;
int gGarageColor;
Robot gRobot;

double gateangle ;
double gatedistance;
double targetangle;
double targetdistance;
double angleSum = 0; // 角度の合計
#ifndef CALC_H_
#define CALC_H_

#include <utility> // std::pair

// (x, y) から対象物の角度と距離を計算する関数
// 引数:
//   x - x座標 (画素など)
//   y - y座標 (画素など)
// 戻り値:
//   first  = angle [deg]
//   second = distance (単位はx, yと同じ)
std::pair<double, double> calculateAngleAndDistance(double x, double y);

#endif // CALC_H_

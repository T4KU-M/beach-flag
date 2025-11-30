#include "goTarget.h"
#include <cmath>
#include <utility> // std::pair
#include <iostream>

constexpr double DEG2RAD = M_PI / 180.0;

std::pair<double, double> calculateAngleAndDistance(double x, double y)
{
    // 1. angle = -30 + (60 * y / 1920)
    double angle_deg = -30.0 + (60.0 * x / 1920.0);
    double angle_rad = angle_deg * DEG2RAD;

    // 2. theta = -2 + (36 * x / 1080)
    double theta_deg = -2.0 + (36.0 * y / 1080.0);
    double theta_rad = theta_deg * DEG2RAD;

    // 3. d_z = x / tan(theta)
    double d_z = 0.0;
    if (std::abs(std::tan(theta_rad)) > 1e-9)
    {
        d_z = 210 / std::tan(theta_rad);    //mm
    }
    else
    {
        d_z = INFINITY; // θ ≈ 0 のとき無限大
    }

    // 4. distance = d_z * cos(angle)
    double distance = d_z * std::cos(angle_rad);

    return {angle_deg*-1, distance};
}

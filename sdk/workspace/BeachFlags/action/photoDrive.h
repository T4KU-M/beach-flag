#pragma once

#include "act.h"
#include <opencv2/opencv.hpp>
#include <string>

class PhotoDrive : public Act
{
public:
    PhotoDrive();

    // 速度と旋回量を決定する（直線走行＋写真撮影）
    void determineSpeedAndSteering() override;

private:
    void capturePhoto();  // 写真撮影処理
};
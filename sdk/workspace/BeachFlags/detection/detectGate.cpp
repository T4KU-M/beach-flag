#include "detectGate.h"
#include "../analysis/estimateGatePosition.h"

// コンストラクタ
DetectGate::DetectGate()
    : mCount(0)
{
}

bool DetectGate::detect()
{
    static estimateGatePosition estimator(0);
    if (!estimator.isOpened()) return false;

    cv::Point center = estimator.find();
    if (center.x != -1 && center.y != -1) {
        // ゲートが検出できた場合
        return true;
    }
    return false;
}
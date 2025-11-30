#pragma once
#include <opencv2/opencv.hpp>

namespace gate
{

    struct FeetDetection
    {
        bool leftFound{false};
        bool rightFound{false};
        cv::Point2f left{-1.0f, -1.0f};
        cv::Point2f right{-1.0f, -1.0f};
        cv::Rect leftRect;
        cv::Rect rightRect;
    };

    // 既存: 足の左右座標を返す
    FeetDetection estimateGateCenter();

    // 追加: 左右の足の「中央座標」を返す（見つからなければ (-1,-1)）
    cv::Point2f estimateGateCenterPoint();

} // namespace gate

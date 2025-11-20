#include "estimateLandingTarget.h"
#include "module_common.h"
#include <iostream>
#include <vector>

// コンストラクタ: カメラを開く
estimateLandingTarget::estimateLandingTarget(int cameraIndex)
{
    cap.open(cameraIndex);
    isCameraReady = cap.isOpened();
    if (!isCameraReady) {
        std::cerr << "Error: Could not open camera with index " << cameraIndex << std::endl;
    }
}

// デストラクタ: カメラ解放
estimateLandingTarget::~estimateLandingTarget()
{
    if (cap.isOpened()) {
        cap.release();
    }
}

// カメラの状態を返す
bool estimateLandingTarget::isOpened() const
{
    return isCameraReady;
}

// 青色ターゲットを検出
cv::Point estimateLandingTarget::find()
{
    if (!isCameraReady) {
        return cv::Point(-1, -1);
    }

    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
        std::cerr << "Warning: Could not grab a frame." << std::endl;
        return cv::Point(-1, -1);
    }

    // HSVに変換
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // 青色範囲でマスク生成
    cv::Mat mask;
    cv::inRange(hsvImage,
                cv::Scalar(lowerHue, lowerSat, lowerVal),
                cv::Scalar(upperHue, upperSat, upperVal),
                mask);

    // ノイズ除去
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN,
                     cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

    // 輪郭検出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    debugImage = frame.clone();

    if (!contours.empty()) {
        // 最大の輪郭を選択
        double maxArea = 0;
        int maxIdx = -1;
        for (int i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea) {
                maxArea = area;
                maxIdx = i;
            }
        }

        if (maxIdx != -1) {
            cv::Rect boundingRect = cv::boundingRect(contours[maxIdx]);
            cv::Point center(
                boundingRect.x + boundingRect.width / 2,
                boundingRect.y + boundingRect.height / 2
            );

            // デバッグ描画
            cv::drawContours(debugImage, contours, maxIdx, cv::Scalar(0, 255, 0), 2);
            cv::rectangle(debugImage, boundingRect, cv::Scalar(255, 0, 0), 2);
            cv::circle(debugImage, center, 5, cv::Scalar(0, 0, 255), -1);

            return center;
        }
    }

    return cv::Point(-1, -1); // 見つからなかった
}

// デバッグ用画像を返す
cv::Mat estimateLandingTarget::getDebugImage()
{
    return debugImage;
}

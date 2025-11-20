#pragma once
#include <opencv2/opencv.hpp>

class estimateLandingTarget
{
public:
    estimateLandingTarget(int cameraIndex);
    ~estimateLandingTarget();

    bool isOpened() const;         // カメラの状態を返す
    cv::Point find();              // 青色ターゲットを検出して中心座標を返す
    cv::Mat getDebugImage();       // デバッグ用画像を取得

private:
    cv::VideoCapture cap;
    bool isCameraReady;
    cv::Mat debugImage;

    // 青色検出用 HSV範囲（環境に応じて調整）
    int lowerHue = 100;
    int upperHue = 140;
    int lowerSat = 100;
    int upperSat = 255;
    int lowerVal = 50;
    int upperVal = 255;
};

#include "imageFeedback.h"
#include <iostream>

ImageFeedback::ImageFeedback(int speedMin, int speedMax, int cameraIndex)
    : Run(speedMin, speedMax), mGateEstimator(cameraIndex), mGateCenter(-1, -1), mFrameWidth(0)
{
    // カメラ画像の幅を取得
    if (mGateEstimator.isOpened()) {
        cv::Mat frame;
        mGateEstimator.cap.read(frame);
        if (!frame.empty()) {
            mFrameWidth = frame.cols;
        }
    }
}

ImageFeedback::~ImageFeedback() {}

void ImageFeedback::determineSteering()
{
    // ゲート中心検出
    mGateCenter = mGateEstimator.find();

    // デフォルトは停止
    mSteering = 0.0;
    mThrottle = 0.0;

    if (mGateCenter.x >= 0 && mFrameWidth > 0) {
        int imageCenterX = mFrameWidth / 2;
        int diff = mGateCenter.x - imageCenterX;

        // ゲインは暫定値（要調整）
        double Kp = 0.005;
        mSteering = diff * Kp;

        // 前進速度は中間値
        mThrottle = (mSpeedMin + mSpeedMax) / 2.0;
    } else {
        // ゲート検出失敗時は安全側で停止
        mSteering = 0.0;
        mThrottle = 0.0;
    }
}
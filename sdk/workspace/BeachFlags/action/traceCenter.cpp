#include "traceCenter.h"
#include <algorithm>

// コンストラクタ (変更なし)
TraceCenter::TraceCenter(int cameraIndex, int imageWidth, double Kp, double Kd, int speedMin, int speedMax)
    : Run(speedMin, speedMax),
      mGateEstimator(cameraIndex),
      mKp(Kp), mKd(Kd), mError(),
      mImageWidth(imageWidth),
      mSteeringMin(-100), mSteeringMax(100), mSteeringOffset(0)
{
}

// 📣 execute() メソッドをここから削除する

// (determineSteering, getDebugImage, isCameraReady は変更なし)
void TraceCenter::determineSteering()
{
    cv::Point targetCenter = mGateEstimator.find();

    if (targetCenter.x == -1) {
        setSteering(mSteeringOffset);
        return;
    }

    const double imageCenterX = mImageWidth / 2.0;
    const double currentError = targetCenter.x - imageCenterX;

    for (int i = 3; i > 0; i--) {
        mError[i] = mError[i - 1];
    }
    mError[0] = currentError;

    double u = 0.0;
    const double dt = 0.01;
    
    u += mKp * mError[0];
    u += mKd * (mError[0] - mError[3]) / (3.0 * dt);

    int steering = mSteeringOffset + static_cast<int>(u);
    steering = std::max(mSteeringMin, std::min(mSteeringMax, steering));

    setSteering(steering);
}

cv::Mat TraceCenter::getDebugImage()
{
    return mGateEstimator.getDebugImage();
}

bool TraceCenter::isCameraReady()
{
    return mGateEstimator.isOpened();
}
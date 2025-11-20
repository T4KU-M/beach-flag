#ifndef TRACE_CENTER_H
#define TRACE_CENTER_H

#include "run.h"
#include "estimateGatePosition.h"
#include <opencv2/opencv.hpp>

class TraceCenter : public Run
{
public:
    TraceCenter(int cameraIndex, int imageWidth, double Kp, double Kd, int speedMin, int speedMax);

    // 📣 上書きするメソッドを execute() から determineSteering() に変更
    void determineSteering() override;

    cv::Mat getDebugImage();
    bool isCameraReady();

private:
    estimateGatePosition mGateEstimator;
    double mKp;
    double mKd;
    double mError[4];
    int mImageWidth;
    int mSteeringMin;
    int mSteeringMax;
    int mSteeringOffset;
};

#endif // TRACE_CENTER_H
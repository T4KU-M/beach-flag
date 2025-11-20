#ifndef TRACE_CENTER_H
#define TRACE_CENTER_H

#include "run.h"
#include "estimateGatePosition.h"
#include <opencv2/opencv.hpp>

// Runクラスを継承することで、actクラスとして振る舞う
class TraceCenter : public Run
{
public:
    TraceCenter(int cameraIndex, int imageWidth, double Kp, double Kd, int speedMin, int speedMax);

    // 📣 Scenarioから毎周期呼び出される実行メソッド
    void execute() override;

    void determineSteering();
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
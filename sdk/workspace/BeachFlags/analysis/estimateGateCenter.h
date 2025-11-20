#ifndef ESTIMATE_GATE_CENTER_H
#define ESTIMATE_GATE_CENTER_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class estimateGateCenter
{
public:
    // コンストラクタ: カメラを開く処理を行う
    estimateGateCenter(int cameraIndex = 0);

    // デストラクタ: カメラを解放する処理を行う
    ~estimateGateCenter();

    // カメラが正常に開かれたか確認する
    bool isOpened() const;

    // 内部カメラからフレームを取得し、ゲート中心を推定する
    cv::Point find();

    // デバッグ画像を取得する
    cv::Mat getDebugImage();

private:
    cv::VideoCapture cap; // 📹 カメラ操作用
    bool isCameraReady;   // カメラの状態
    cv::Mat debugImage;   // デバッグ用画像

    // 検出するゲート色範囲 (HSV)
    int lowerHue = 0, upperHue = 10;
    int lowerSat = 120, upperSat = 255;
    int lowerVal = 120, upperVal = 255;
};

#endif // ESTIMATE_GATE_CENTER_H

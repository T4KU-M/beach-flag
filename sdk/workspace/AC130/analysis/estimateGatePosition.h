#ifndef ESTIMATE_GATE_POSITION_H
#define ESTIMATE_GATE_POSITION_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

class estimateGatePosition
{
public:
    // コンストラクタ: カメラを開く処理を行う
    estimateGatePosition(int cameraIndex = 0);

    // デストラクタ: カメラを解放する処理を行う
    ~estimateGatePosition();

    // カメラが正常に開かれたか確認する
    bool isOpened() const;

    // 内部のカメラからフレームを取得し、ゲートの中心座標を推定する
    cv::Point find();

    // デバッグ画像を取得する
    cv::Mat getDebugImage();

private:
    cv::VideoCapture cap; // 📹 カメラ操作用のオブジェクト
    bool isCameraReady;   // カメラの状態を保存するフラグ
    cv::Mat debugImage;   // デバッグ画像を保存する変数

    // 検出するゲートの色範囲 (HSV)
    int lowerHue = 0, upperHue = 10;
    int lowerSat = 120, upperSat = 255;
    int lowerVal = 120, upperVal = 255;
};

#endif // ESTIMATE_GATE_POSITION_H
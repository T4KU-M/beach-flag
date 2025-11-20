#pragma once
// 画像フィードバックを用いてゲートへ向かう動作クラス
class ImageFeedback : public Run
{
public:
    ImageFeedback(int speedMin, int speedMax, int cameraIndex);
    virtual ~ImageFeedback();

protected:
    void determineSteering() override;  // ステアリングを画像フィードバックで決定

private:
    estimateGatePosition mGateEstimator;
    cv::Point mGateCenter;   // 検出したゲートの中心
    int mFrameWidth;         // カメラの幅（画像サイズ）
};

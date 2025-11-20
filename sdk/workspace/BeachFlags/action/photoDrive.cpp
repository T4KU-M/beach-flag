#include "act.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

class PhotoDrive : public Act
{
public:
    PhotoDrive() : Act() {}

    // 速度と旋回量を決定する（直線走行＋写真撮影）
    void determineSpeedAndSteering() override
    {
        // 直線走行（速度 50、旋回量 0）
        setSpeed(50);
        setSteering(0);

        // 写真撮影処理
        capturePhoto();
    }

private:
    void capturePhoto()
    {
        cv::VideoCapture cap(0, cv::CAP_V4L2);
        if (!cap.isOpened())
        {
            std::cerr << "カメラを開けませんでした" << std::endl;
            return;
        }

        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "フレームが空です" << std::endl;
            return;
        }

        // タイムスタンプ付きファイル名を生成
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << "photo_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".jpg";

        // 保存
        std::vector<int> param{cv::IMWRITE_JPEG_QUALITY, 95};
        if (!cv::imwrite(oss.str(), frame, param))
        {
            std::cerr << "写真保存に失敗しました" << std::endl;
            return;
        }

        std::cout << "撮影成功: " << oss.str() << std::endl;
        cap.release();
    }
};
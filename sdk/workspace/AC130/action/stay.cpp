#include "stay.h"

// コンストラクタ
Stay::Stay() : Act()
{
}

// 速度と旋回量を決定する
void Stay::determineSpeedAndSteering()
{
	setSpeed(0);		// 速度をゼロにする
	setSteering(0);		// 旋回量をゼロにする

	//20250819
	// OpenCV が使えるか確認するテスト
    /*
    cv::VideoCapture cap(0);  // 0はデフォルトカメラ
    if (!cap.isOpened()) {
        std::cerr << "カメラを開けませんでした" << std::endl;
    }

    cv::Mat frame;
    cap >> frame;  // 1枚キャプチャ

    if (frame.empty()) {
        std::cerr << "フレームが空です" << std::endl;
    }

    // キャプチャ画像を保存して確認できるようにする
    cv::imwrite("capture_test.jpg", frame);

    std::cout << "OpenCVで写真を撮影して capture_test.jpg に保存しました！" << std::endl;
    */
}

#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap(0); // カメラデバイス0をオープン
    if (!cap.isOpened()) {
        std::cerr << "カメラが利用できません" << std::endl;
        return 1;
    }

    cv::Mat frame;
    std::cout << "Enterキーで撮影、qで終了します" << std::endl;

    while (true) {
        while(1) {
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "フレームが取得できません" << std::endl;
                break;
            }

            std::string filename = "sample.jpg";
            cv::imwrite(filename, frame);
            std::cout << "画像を保存しました: " << filename << std::endl;
        }
    }

    cap.release();
    return 0;
}

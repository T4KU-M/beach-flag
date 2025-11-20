// record_mjpg.cpp
#include <opencv2/opencv.hpp>
#include <chrono>
#include <iostream>

int main() {
    // --- 1. カメラを開く（/dev/video0） ---------------------------------
    cv::VideoCapture cap(0, cv::CAP_V4L2);             // V4L2 指定で安定
    if (!cap.isOpened()) { std::cerr << "Camera open failed\n"; return -1; }

    // 解像度や FPS を必要に応じて設定（カメラが拒否する場合もあり）
    int width  = 1280, height = 720, fps = 30;
    cap.set(cv::CAP_PROP_FRAME_WIDTH , width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FPS         , fps);
    cap.set(cv::CAP_PROP_FOURCC      , cv::VideoWriter::fourcc('M','J','P','G')); // MJPG 転送

    // 実際に設定された値を取得（うまく通らなかった場合に備えて）
    width  = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH ));
    height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    fps    = static_cast<int>(cap.get(cv::CAP_PROP_FPS         ));
    std::cout << "Capture: " << width << "x" << height << " @" << fps << "fps\n";

    // --- 2. MJPG で書き出す VideoWriter を用意 ---------------------------
    // 拡張子は .avi が無難（MJPG コーデック付きの Motion-JPEG）
    cv::VideoWriter writer("output_mjpg.avi",
                           cv::VideoWriter::fourcc('M','J','P','G'),
                           fps, cv::Size(width, height));
    if (!writer.isOpened()) { std::cerr << "Writer open failed\n"; return -1; }

    // --- 3. 10 秒間だけフレームを回収して保存 ---------------------------
    const auto start = std::chrono::steady_clock::now();
    cv::Mat frame;
    while (true) {
        if (!cap.read(frame) || frame.empty()) continue;  // 取得
        writer.write(frame);                              // 保存

        // 10 秒経過で終了
        auto elapsed = std::chrono::steady_clock::now() - start;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 10)
            break;
    }
    std::cout << "Saved to output_mjpg.avi (10 s)\n";
    return 0;
}
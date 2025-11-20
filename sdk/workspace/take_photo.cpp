#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

int main()
{
    // 1. カメラを開く（第 2 引数を付けて V4L2 を明示）
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "Error: camera open failed\n";
        return -1;
    }

    // 2. 好みの解像度やフォーマットを要求（通らなければ実際の値が使われる）
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G')); // 転送を MJPEG に

    // 3. フレームを 1 枚取得
    cv::Mat frame;
    // read() が true を返すまでリトライ（起動直後は空フレームになる機種がある）
    for (int i = 0; i < 10 && !cap.read(frame); ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    if (frame.empty())
    {
        std::cerr << "Error: failed to grab frame\n";
        return -1;
    }

    // 4. タイムスタンプ付きファイル名を生成
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << "photo_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".jpg";

    // 5. 保存（品質 95 に設定）
    std::vector<int> param{cv::IMWRITE_JPEG_QUALITY, 95};
    if (!cv::imwrite(oss.str(), frame, param))
    {
        std::cerr << "Error: imwrite failed\n";
        return -1;
    }

    std::cout << "Saved: " << oss.str() << std::endl;
    return 0;
}
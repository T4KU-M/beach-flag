#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string filename = (argc > 1) ? argv[1] : "sampleblue.jpg";
    cv::Mat src = cv::imread(filename, cv::IMREAD_COLOR);
    if (src.empty()) {
        std::cerr << "画像が読み込めません: " << filename << std::endl;
        return 1;
    }

    int lower = 100; // 初期下限（青の値）
    int upper = 255; // 初期上限（青の値）

    std::cout << "青チャンネル抽出範囲を入力してください (下限 上限, 0～255)、終了はq:" << std::endl;

    while (true) {
        std::cout << "Range = [" << lower << ", " << upper << "] >> ";
        std::string input;
        std::cin >> input;

        if (input == "q") break;

        try {
            int l = std::stoi(input);
            std::cin >> input;
            int u = std::stoi(input);

            if (l < 0 || l > 255 || u < 0 || u > 255 || l > u) {
                std::cout << "0～255の範囲かつ下限<=上限で入力してください" << std::endl;
                continue;
            }
            lower = l;
            upper = u;
        } catch (...) {
            std::cout << "数字を2つ入力してください" << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        // 青チャンネルのみ抽出
        std::vector<cv::Mat> channels;
        cv::split(src, channels); // BGR順
        cv::Mat blue = channels[0]; // 青チャンネル

        cv::Mat mask;
        cv::inRange(blue, lower, upper, mask);

        std::string outname = "blueRange_" + std::to_string(lower) + "_" + std::to_string(upper) + ".jpg";
        cv::imwrite(outname, mask);
        std::cout << "保存しました: " << outname << std::endl;
    }

    std::cout << "終了します" << std::endl;
    return 0;
}
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string filename = (argc > 1) ? argv[1] : "sample.jpg";
    cv::Mat src = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    if (src.empty()) {
        std::cerr << "画像が読み込めません: " << filename << std::endl;
        return 1;
    }

    int lower = 120; // 初期下限
    int upper = 180; // 初期上限

    std::cout << "灰色抽出範囲を入力してください (下限 上限, 0～255)、終了はq:" << std::endl;

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
            // cinのバッファクリア
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        // 灰色範囲抽出
        cv::Mat mask;
        cv::inRange(src, lower, upper, mask);

        // 保存
        std::string outname = "grayrange_" + std::to_string(lower) + "_" + std::to_string(upper) + ".jpg";
        cv::imwrite(outname, mask);
        std::cout << "保存しました: " << outname << std::endl;
    }

    std::cout << "終了します" << std::endl;
    return 0;
}

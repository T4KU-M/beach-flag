#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>

static std::string timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&t));
    return std::string(buf);
}

int main(int argc, char** argv) {
    std::string filename = (argc > 1) ? argv[1] : "sample.jpg";
    cv::Mat src = cv::imread(filename, cv::IMREAD_COLOR);
    if (src.empty()) {
        std::cerr << "画像が読み込めません: " << filename << std::endl;
        return 1;
    }

    std::cout << "画像: " << filename << std::endl;
    std::cout << "操作: Trackbarで閾値調整 / s:保存 / q,ESC:終了" << std::endl;

    // BGR -> Lab (8U: L[0..255], a[0..255], b[0..255]、a,bは128がゼロ点)
    cv::Mat lab;
    cv::cvtColor(src, lab, cv::COLOR_BGR2Lab);

    // 初期パラメータ（経験的デフォルト）
    int l_min = 20;     // 暗すぎる黒を除外
    int l_max = 235;    // 明るすぎる白を除外
    int c_max = 12;     // 低クロマ閾値（目安: 10〜25）

    // モルフォロジー（ノイズ除去/穴埋め）
    int morph_radius = 1; // 0で無効、1->3x3, 2->5x5...
    int morph_open = 1;   // 1でOpen
    int morph_close = 0;  // 1でClose

    // 画面サイズに応じて表示縮小
    double disp_scale = 1.0;
    const int max_dim = 1000;
    if (std::max(src.cols, src.rows) > max_dim) {
        disp_scale = static_cast<double>(max_dim) / static_cast<double>(std::max(src.cols, src.rows));
    }

    cv::namedWindow("controls", cv::WINDOW_NORMAL);
    cv::resizeWindow("controls", 520, 240);
    cv::createTrackbar("Lab: L_min", "controls", &l_min, 255);
    cv::createTrackbar("Lab: L_max", "controls", &l_max, 255);
    cv::createTrackbar("Lab: C_max", "controls", &c_max, 100);
    cv::createTrackbar("Morph radius", "controls", &morph_radius, 10);
    cv::createTrackbar("Use Open (1/0)", "controls", &morph_open, 1);
    cv::createTrackbar("Use Close (1/0)", "controls", &morph_close, 1);

    cv::namedWindow("mask", cv::WINDOW_NORMAL);
    cv::namedWindow("result", cv::WINDOW_NORMAL);

    auto applyMorph = [&](cv::Mat& mask) {
        if (morph_radius > 0) {
            cv::Mat kernel = cv::getStructuringElement(
                cv::MORPH_ELLIPSE, cv::Size(2 * morph_radius + 1, 2 * morph_radius + 1));
            if (morph_open)  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
            if (morph_close) cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
        }
    };

    while (true) {
        // 分離
        cv::Mat ch[3];
        cv::split(lab, ch); // L, a, b (8U)
        cv::Mat L = ch[0], a = ch[1], b = ch[2];

        // a,bを32Fにして128を原点に移動 -> クロマ = sqrt(a^2 + b^2)
        cv::Mat a32, b32, chroma;
        a.convertTo(a32, CV_32F);
        b.convertTo(b32, CV_32F);
        a32 = a32 - 128.0f;
        b32 = b32 - 128.0f;
        cv::magnitude(a32, b32, chroma); // 32F

        int Lmin = std::max(0, std::min(255, l_min));
        int Lmax = std::max(0, std::min(255, l_max));
        if (Lmin > Lmax) std::swap(Lmin, Lmax);
        float Cmax = static_cast<float>(std::max(0, c_max));

        // L範囲マスク
        cv::Mat lmask;
        cv::inRange(L, Lmin, Lmax, lmask);

        // 低クロママスク（小さいほど白 = 抽出）
        cv::Mat cmask;
        cv::threshold(chroma, cmask, Cmax, 255.0, cv::THRESH_BINARY_INV);
        cmask.convertTo(cmask, CV_8U);

        // ANDでグレー領域
        cv::Mat mask;
        cv::bitwise_and(lmask, cmask, mask);

        // モルフォロジー
        applyMorph(mask);

        // 見やすい可視化（抽出領域を半透明グリーンでハイライト）
        cv::Mat result = src.clone();
        cv::Mat overlay = src.clone();
        cv::Mat green(result.size(), result.type(), cv::Scalar(0, 255, 0));
        cv::addWeighted(src, 0.5, green, 0.5, 0.0, overlay);
        overlay.copyTo(result, mask);

        // テキスト描画
        char buf[160];
        std::snprintf(buf, sizeof(buf), "Lab: L=[%d,%d], C_max=%d, Morph(r=%d, open=%d, close=%d)",
                      Lmin, Lmax, c_max, morph_radius, morph_open, morph_close);
        cv::putText(result, buf, {10, 25}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {0, 0, 0}, 3, cv::LINE_AA);
        cv::putText(result, buf, {10, 25}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {255, 255, 255}, 1, cv::LINE_AA);

        // 表示（必要なら縮小）
        cv::Mat dispMask = mask, dispRes = result;
        if (disp_scale != 1.0) {
            cv::resize(mask, dispMask, cv::Size(), disp_scale, disp_scale, cv::INTER_NEAREST);
            cv::resize(result, dispRes, cv::Size(), disp_scale, disp_scale, cv::INTER_AREA);
        }
        cv::imshow("mask", dispMask);
        cv::imshow("result", dispRes);

        int key = cv::waitKey(30);
        if (key == 27 || key == 'q' || key == 'Q') {
            break;
        } else if (key == 's' || key == 'S') {
            std::string ts = timestamp();
            std::string base = "gray_lab_L" + std::to_string(Lmin) + "-" + std::to_string(Lmax) +
                               "_C" + std::to_string(c_max) +
                               "_r" + std::to_string(morph_radius) + "_" + ts;
            cv::imwrite(base + "_mask.png", mask);
            cv::imwrite(base + "_result.png", result);
            std::cout << "保存: " << base + "_mask.png, " << base + "_result.png" << std::endl;
        }
    }

    std::cout << "終了します" << std::endl;
    return 0;
}

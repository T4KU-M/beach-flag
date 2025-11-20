#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <cstdio>
#include <algorithm>

static int clamp(int v, int lo, int hi){ return std::max(lo, std::min(hi, v)); }
static int toOdd(int k){ return (k <= 0) ? 0 : (2 * k + 1); }

struct DetectionResult {
    bool found{false};
    cv::Point2f centroid{0.0f, 0.0f};
    cv::RotatedRect ellipse;
    double contourArea{0.0};
    double ellipseArea{0.0};
    double purity{0.0};
};

static double ellipseAreaOf(const cv::RotatedRect& e) {
    double a = static_cast<double>(e.size.width) * 0.5;
    double b = static_cast<double>(e.size.height) * 0.5;
    return CV_PI * a * b;
}

static cv::Mat makeEllipseMask(const cv::Size& size, const cv::RotatedRect& e) {
    cv::Mat m = cv::Mat::zeros(size, CV_8UC1);
    cv::ellipse(m, e, cv::Scalar(255), cv::FILLED, cv::LINE_AA);
    return m;
}

static DetectionResult detectLargestBlueEllipse(const cv::Mat& bgr,
                                                int Hmin=95, int Hmax=140,
                                                int Smin=60, int Smax=255,
                                                int Vmin=40, int Vmax=255,
                                                int blurK=1, int openK=1, int closeK=1)
{
    DetectionResult best;

    // 前処理（ブラー）
    cv::Mat work = bgr.clone();
    int kBlur = toOdd(blurK);
    if (kBlur >= 3) cv::GaussianBlur(work, work, cv::Size(kBlur, kBlur), 0);

    // HSV マスク
    cv::Mat hsv; cv::cvtColor(work, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv,
                cv::Scalar(clamp(Hmin,0,179), clamp(Smin,0,255), clamp(Vmin,0,255)),
                cv::Scalar(clamp(Hmax,0,179), clamp(Smax,0,255), clamp(Vmax,0,255)),
                mask);

    // モルフォロジー処理
    if (toOdd(openK) >= 3) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(toOdd(openK), toOdd(openK)));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    }
    if (toOdd(closeK) >= 3) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(toOdd(closeK), toOdd(closeK)));
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
    }

    // 輪郭検出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double imgArea = static_cast<double>(bgr.cols) * bgr.rows;
    double minArea = std::max(30.0, imgArea * 0.00005); // 少し緩め
    double maxArea = imgArea * 0.5;

    double bestArea = 0.0;

    for (const auto& c : contours) {
        if (c.size() < 5) continue;

        double a = cv::contourArea(c);
        if (a < minArea || a > maxArea) continue;

        cv::RotatedRect e;
        try {
            e = cv::fitEllipse(c);
        } catch (...) {
            continue;
        }

        double ew = std::max<double>(1.0, e.size.width);
        double eh = std::max<double>(1.0, e.size.height);
        double axisRatio = std::min(ew, eh) / std::max(ew, eh);
        if (axisRatio < 0.3) continue;

        double eArea = ellipseAreaOf(e);
        if (eArea <= 0.0) continue;

        cv::Mat eMask = makeEllipseMask(mask.size(), e);
        cv::Mat inside;
        cv::bitwise_and(mask, eMask, inside);
        double ellipsePix = (double)cv::countNonZero(eMask);
        double bluePix    = (double)cv::countNonZero(inside);
        double purity     = (ellipsePix > 0.0) ? (bluePix / ellipsePix) : 0.0;

        if (purity < 0.05) continue;

        double fill = a / std::max(1.0, eArea);
        if (fill < 0.05 || fill > 5.0) continue;

        std::cout << "Contour area=" << a
                  << " axisRatio=" << axisRatio
                  << " purity=" << purity
                  << " fill=" << fill
                  << std::endl;

        if (a > bestArea) {
            bestArea = a;
            best.found = true;
            best.contourArea = a;
            best.ellipseArea = eArea;
            best.purity = purity;
            best.ellipse = e;
            best.centroid = e.center;
        }
    }

    return best;
}

static void drawResultPreview(const cv::Mat& src, const cv::Mat& mask, const DetectionResult& r, const std::string& outName) {
    cv::Mat maskColor;
    cv::applyColorMap(mask, maskColor, cv::COLORMAP_OCEAN);

    cv::Mat overlay = src.clone();
    if (r.found) {
        cv::ellipse(overlay, r.ellipse, cv::Scalar(0,255,0), 2, cv::LINE_AA);
        cv::drawMarker(overlay, r.centroid, cv::Scalar(0,0,255), cv::MARKER_CROSS, 18, 2, cv::LINE_AA);
        char buf[256];
        std::snprintf(buf, sizeof(buf), "centroid(%.1f,%.1f) purity=%.2f", r.centroid.x, r.centroid.y, r.purity);
        cv::putText(overlay, buf, r.centroid + cv::Point2f(8,-8), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(50,220,50), 2, cv::LINE_AA);
    } else {
        cv::putText(overlay, "NO TARGET FOUND", cv::Point(20,40), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0,0,255), 2, cv::LINE_AA);
    }

    int H = src.rows;
    cv::Mat left = src.clone();
    cv::Mat maskAdj, overlayAdj;
    cv::resize(maskColor, maskAdj, cv::Size((int)std::round(mask.cols*(H/(double)mask.rows)), H));
    cv::resize(overlay, overlayAdj, cv::Size((int)std::round(overlay.cols*(H/(double)overlay.rows)), H));

    cv::Mat side;
    cv::hconcat(std::vector<cv::Mat>{left, maskAdj, overlayAdj}, side);
    cv::imwrite(outName, side);
}

int main(int argc, char** argv) {
    std::string filename = (argc > 1) ? argv[1] : "sample.jpg";

    cv::Mat src = cv::imread(filename);
    if (src.empty()) {
        std::cerr << "画像を読み込めません: " << filename << std::endl;
        return 1;
    }

    int Hmin=95,Hmax=140,Smin=60,Smax=255,Vmin=40,Vmax=255;
    int blurK=1,openK=1,closeK=1;

    std::vector<int> vals;
    for (int i=2;i<argc;++i){ try{ vals.push_back(std::stoi(argv[i])); } catch(...){} }
    if (vals.size()>=6){ Hmin=vals[0]; Hmax=vals[1]; Smin=vals[2]; Smax=vals[3]; Vmin=vals[4]; Vmax=vals[5]; }
    if (vals.size()>=7) blurK=vals[6];
    if (vals.size()>=8) openK=vals[7];
    if (vals.size()>=9) closeK=vals[8];

    DetectionResult r = detectLargestBlueEllipse(src,Hmin,Hmax,Smin,Smax,Vmin,Vmax,blurK,openK,closeK);

    // mask を作り直す
    cv::Mat hsv; cv::cvtColor(src,hsv,cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(clamp(Hmin,0,179),clamp(Smin,0,255),clamp(Vmin,0,255)),
                     cv::Scalar(clamp(Hmax,0,179),clamp(Smax,0,255),clamp(Vmax,0,255)), mask);

    std::string outName = "out_landing_target_" + std::filesystem::path(filename).stem().string() + ".jpg";
    drawResultPreview(src, mask, r, outName);

    std::cout << "{"
              << "\"found\":" << (r.found?"true":"false")
              << ",\"centroid\":{\"x\":" << r.centroid.x << ",\"y\":" << r.centroid.y << "}"
              << ",\"purity\":" << r.purity
              << ",\"contour_area\":" << r.contourArea
              << ",\"ellipse_area\":" << r.ellipseArea
              << ",\"preview\":\"" << outName << "\""
              << "}" << std::endl;

    return 0;
}

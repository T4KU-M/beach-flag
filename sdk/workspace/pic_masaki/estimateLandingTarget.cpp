#include "estimateLandingTarget.h"

#include <iostream>
#include <vector>

#include <cmath>
#include <cstdio>
#include <algorithm>

#define MINH 95
#define MAXH 145
#define MINS 200
#define MAXS 255
#define MINV 115
#define MAXV 255

namespace target
{
    //////////////////////////////////
    extern std::string filename;
    extern std::string outName;
    extern DetectionResult result;
    extern cv::Mat src, hsv, mask;

    extern DetectionResult detect(const cv::Mat &bgr);
    extern void drawResultPreview(const cv::Mat &src, const cv::Mat &mask,const DetectionResult &r, const std::string &outName);

    static int clamp(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); };
    static int toOdd(int k) { return (k <= 0) ? 0 : (2 * k + 1); };
    static double ellipseAreaOf(const cv::RotatedRect &e);
    static cv::Mat makeEllipseMask(const cv::Size &size, const cv::RotatedRect &e);
    /////////////
    DetectionResult result;
    std::string filename = "sample.jpg";
    std::string outName = "out_landing_target.jpg";
    cv::Mat src;
    cv::Mat hsv;
    cv::Mat mask;

    //////////////////////////////全体の処理//////////////////////////////////////////////////////
    cv::Point2f estimatelandingtarget()
    {
        src = cv::imread(filename);
        if (src.empty())
        {
            // std::cerr << "画像を読み込めません: " << filename << std::endl; // 結果表示をコメントアウト
            return cv::Point2f(-1.0f, -1.0f);
        }

        result = detect(src);

        cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv,
                    cv::Scalar(clamp(MINH, 0, 179), clamp(MINS, 0, 255), clamp(MINV, 0, 255)),
                    cv::Scalar(clamp(MAXH, 0, 179), clamp(MAXS, 0, 255), clamp(MAXV, 0, 255)),
                    mask);

        drawResultPreview(src, mask, result, outName);

        // std::cout << "{"
        //           << "\"found\":" << (result.found ? "true" : "false")
        //           << ",\"centroid\":{\"x\":" << result.centroid.x << ",\"y\":" << result.centroid.y << "}"
        //           << ",\"purity\":" << result.purity
        //           << ",\"contour_area\":" << result.contourArea
        //           << ",\"ellipse_area\":" << result.ellipseArea
        //           << ",\"preview\":\"" << outName << "\""
        //           << "}" << std::endl;
        std::cout << result.centroid.x << ", " << result.centroid.y << std::endl;

        return result.centroid;
    }
    //////////////////////////////全体の処理//////////////////////////////////////////////////////

    // 楕円の面積を計算////////////////////////////////////////////////////////////////////////////
    double ellipseAreaOf(const cv::RotatedRect &e)
    {
        double a = static_cast<double>(e.size.width) * 0.5;
        double b = static_cast<double>(e.size.height) * 0.5;
        return CV_PI * a * b;
    }
    // 楕円の面積を計算////////////////////////////////////////////////////////////////////////////

    //// 楕円のマスクを作成////////////////////////////////////////////////////////////////////////////
    cv::Mat makeEllipseMask(const cv::Size &size, const cv::RotatedRect &e)
    {
        cv::Mat m = cv::Mat::zeros(size, CV_8UC1);
        cv::ellipse(m, e, cv::Scalar(255), cv::FILLED, cv::LINE_AA);
        return m;
    }
    //////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////ターゲット検出のメイン処理////////////////////////////////////////////////////
    DetectionResult detect(const cv::Mat &bgr)
    {
        DetectionResult best;

        // 前処理（ブラー）
        cv::Mat work = bgr.clone();
        int kBlur = toOdd(1);
        if (kBlur >= 3)
            cv::GaussianBlur(work, work, cv::Size(kBlur, kBlur), 0);

        // HSV マスク
        cv::Mat hsv;
        cv::cvtColor(work, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask;
        cv::inRange(hsv,
                    cv::Scalar(clamp(MINH, 0, 179), clamp(MINS, 0, 255), clamp(MINV, 0, 255)),
                    cv::Scalar(clamp(MAXH, 0, 179), clamp(MAXS, 0, 255), clamp(MAXV, 0, 255)),
                    mask);

        // モルフォロジー処理
        if (toOdd(1) >= 3)
        {
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(toOdd(1), toOdd(1)));
            cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
        }
        if (toOdd(1) >= 3)
        {
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(toOdd(1), toOdd(1)));
            cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
        }

        // 輪郭検出
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        double imgArea = static_cast<double>(bgr.cols) * bgr.rows;
        double minArea = std::max(30.0, imgArea * 0.00005);
        double maxArea = imgArea * 0.5;

        double bestArea = 0.0;

        for (const auto &c : contours)
        {
            if (c.size() < 5)
                continue;

            double a = cv::contourArea(c);
            if (a < minArea || a > maxArea)
                continue;

            cv::RotatedRect e;
            try
            {
                e = cv::fitEllipse(c);
            }
            catch (...)
            {
                continue;
            }

            double ew = std::max<double>(1.0, e.size.width);
            double eh = std::max<double>(1.0, e.size.height);
            double axisRatio = ew / eh;
            if (axisRatio < 0.05 || 1.5 < axisRatio){
            printf("axisRatio=%f\n", axisRatio);
                 continue;}

            double eArea = ellipseAreaOf(e);

            if (eArea <= 1000.0)
                continue;
                    printf("eArea=%f\n" ,eArea);
            cv::Mat eMask = makeEllipseMask(mask.size(), e);
            cv::Mat inside;
            cv::bitwise_and(mask, eMask, inside);
            double ellipsePix = static_cast<double>(cv::countNonZero(eMask));
            double bluePix = static_cast<double>(cv::countNonZero(inside));
            double purity = (ellipsePix > 0.0) ? (bluePix / ellipsePix) : 0.0;

            if (purity < 0.6)
                continue;

            double fill = a / std::max(1.0, eArea);
            if (fill < 0.6 || 1.5 < fill)
                continue;

            // 検出した輪郭（contour）に関する情報をデバッグ用に表示
            // std::cout << "Contour area=" << a
            //           << " axisRatio=" << axisRatio
            //           << " purity=" << purity
            //           << " fill=" << fill
            //           << std::endl;

            if (a > bestArea)
            {
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////結果の描画と保存////////////////////////////////////////////////////
    void drawResultPreview(const cv::Mat &src, const cv::Mat &mask, const DetectionResult &r, const std::string &outName)
    {
        cv::Mat maskColor;
        cv::applyColorMap(mask, maskColor, cv::COLORMAP_OCEAN);

        cv::Mat overlay = src.clone();
        if (r.found)
        {
            cv::ellipse(overlay, r.ellipse, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
            cv::drawMarker(overlay, r.centroid, cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 18, 2, cv::LINE_AA);
            char buf[256];
            std::snprintf(buf, sizeof(buf), "centroid(%.1f,%.1f) purity=%.2f", r.centroid.x, r.centroid.y, r.purity);
            cv::putText(overlay, buf, r.centroid + cv::Point2f(8, -8), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(50, 220, 50), 2, cv::LINE_AA);
        }
        else
        {
            cv::putText(overlay, "NO TARGET FOUND", cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        }

        int H = src.rows;
        cv::Mat left = src.clone();
        cv::Mat maskAdj, overlayAdj;
        cv::resize(maskColor, maskAdj, cv::Size(static_cast<int>(std::round(mask.cols * (H / static_cast<double>(mask.rows)))), H));
        cv::resize(overlay, overlayAdj, cv::Size(static_cast<int>(std::round(overlay.cols * (H / static_cast<double>(overlay.rows)))), H));

        cv::Mat side;
        cv::hconcat(std::vector<cv::Mat>{left, maskAdj, overlayAdj}, side);
        cv::imwrite(outName, side);
    }
    /////////////////////////////結果の描画と保存////////////////////////////////////////////////////
} // namespace target

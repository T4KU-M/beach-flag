#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <cmath>

static inline int clamp(int v, int lo, int hi){ return std::max(lo, std::min(hi, v)); }
static inline int toOdd(int k){ return (k <= 0) ? 0 : (2 * k + 1); }

struct Pillar {
    cv::RotatedRect rect;
    cv::Point2f centroid;
    double area{0.0};
    double purity{0.0};
    double aspect{0.0};
    double meanSat{0.0};
    double meanA{0.0};
    double meanB{0.0};
};

struct DetectionResult {
    bool foundTwo{false};
    Pillar left;
    Pillar right;
    cv::Point2f gateCenter{0.0f, 0.0f};
    double separationPx{0.0};
    double separationNorm{0.0};
    double confidence{0.0};
    std::string previewFile;
};

static cv::Mat makeRectMask(const cv::Size& size, const cv::RotatedRect& r){
    cv::Mat m = cv::Mat::zeros(size, CV_8UC1);
    cv::Point2f pts[4]; r.points(pts);
    std::vector<cv::Point> poly;
    for(int i=0;i<4;i++)
        poly.emplace_back((int)std::round(pts[i].x),(int)std::round(pts[i].y));
    cv::fillConvexPoly(m, poly, cv::Scalar(255));
    return m;
}

// グレー候補マスク生成（多条件 AND）
static cv::Mat buildGrayMask(const cv::Mat& bgr,
                             int Hmin, int Hmax, int Smin, int Smax, int Vmin, int Vmax,
                             int deltaMax, int aThr, int bThr,
                             bool rejectGreenHue, int greenHmin, int greenHmax, int greenSMin)
{
    cv::Mat hsv; cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    cv::Mat maskHSV;
    cv::inRange(hsv,
                cv::Scalar(clamp(Hmin,0,179), clamp(Smin,0,255), clamp(Vmin,0,255)),
                cv::Scalar(clamp(Hmax,0,179), clamp(Smax,0,255), clamp(Vmax,0,255)),
                maskHSV);

    // Lab で a,b の閾値
    cv::Mat lab; cv::cvtColor(bgr, lab, cv::COLOR_BGR2Lab);
    std::vector<cv::Mat> labCh; cv::split(lab, labCh); // L, a, b
    cv::Mat absA, absB;
    cv::absdiff(labCh[1], cv::Scalar(128), absA); // a*: 0..255 中心128
    cv::absdiff(labCh[2], cv::Scalar(128), absB);
    // 条件: |a*| < aThr, |b*| < bThr
    cv::Mat maskAB = (absA < aThr) & (absB < bThr);

    // RGB Δ（彩度別指標）
    cv::Mat bgrSplit[3]; cv::split(bgr, bgrSplit);
    cv::Mat maxCh, minCh;
    cv::max(bgrSplit[0], bgrSplit[1], maxCh);
    cv::max(maxCh, bgrSplit[2], maxCh);
    cv::min(bgrSplit[0], bgrSplit[1], minCh);
    cv::min(minCh, bgrSplit[2], minCh);
    cv::Mat delta; cv::subtract(maxCh, minCh, delta);
    cv::Mat maskDelta = (delta < deltaMax);

    // 緑 Hue 除外（低彩度例外）
    cv::Mat maskGreenReject = cv::Mat::ones(bgr.size(), CV_8UC1)*255;
    if (rejectGreenHue) {
        // hsv channels
        std::vector<cv::Mat> hsvCh; cv::split(hsv, hsvCh);
        cv::Mat h = hsvCh[0];
        cv::Mat s = hsvCh[1];
        // 緑領域で Saturation がある程度（greenSMin 以上）→ 除外
        cv::Mat green = (h >= greenHmin) & (h <= greenHmax) & (s >= greenSMin);
        // maskGreenReject で「緑」ピクセルを 0 に
        maskGreenReject.setTo(0, green);
    }

    cv::Mat grayMask = maskHSV & maskAB & maskDelta & maskGreenReject;
    return grayMask;
}

static DetectionResult detectGrayGateCenter(const cv::Mat& bgr,
                                            // HSV 基本
                                            int Hmin, int Hmax, int Smin, int Smax, int Vmin, int Vmax,
                                            // 追加 色差/彩度特性
                                            int deltaMax, int aThr, int bThr,
                                            bool rejectGreenHue, int greenHmin, int greenHmax, int greenSMin,
                                            // 形態・幾何
                                            int blurK, int openK, int closeK,
                                            double minAreaRatio, double maxAreaRatio,
                                            double minAspect, double minPurity, double minSepNorm)
{
    DetectionResult res;
    if (bgr.empty()) return res;

    // 前処理
    cv::Mat work = bgr.clone();
    int kBlur = toOdd(blurK);
    if (kBlur >= 3) cv::GaussianBlur(work, work, cv::Size(kBlur,kBlur), 0);

    // 多条件グレー候補マスク
    cv::Mat initialMask = buildGrayMask(work,
                                        Hmin,Hmax,Smin,Smax,Vmin,Vmax,
                                        deltaMax,aThr,bThr,
                                        rejectGreenHue,greenHmin,greenHmax,greenSMin);

    // 形態学
    cv::Mat mask = initialMask.clone();
    int kOpen = toOdd(openK);
    int kClose = toOdd(closeK);
    if (kOpen >= 3) {
        cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kOpen,kOpen));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, k);
    }
    if (kClose >= 3) {
        cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kClose,kClose));
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, k);
    }

    // 輪郭
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double imgArea = (double)bgr.cols * bgr.rows;
    double minArea = std::max(80.0, imgArea * minAreaRatio);
    double maxArea = imgArea * maxAreaRatio;

    // HSV / Lab を再利用
    cv::Mat hsv; cv::cvtColor(work, hsv, cv::COLOR_BGR2HSV);
    cv::Mat lab; cv::cvtColor(work, lab, cv::COLOR_BGR2Lab);
    std::vector<cv::Mat> hsvCh, labCh;
    cv::split(hsv, hsvCh);
    cv::split(lab, labCh);

    std::vector<Pillar> pillars;

    for (auto &c : contours){
        if (c.size()<4) continue;
        double a = cv::contourArea(c);
        if (a < minArea || a > maxArea) continue;

        cv::RotatedRect r;
        try { r = cv::minAreaRect(c); } catch(...) { continue; }

        double w = std::max(1.0,(double)r.size.width);
        double h = std::max(1.0,(double)r.size.height);
        if (w > h) std::swap(w,h);
        double aspect = h / w;
        if (aspect < minAspect) continue;

        cv::Mat rMask = makeRectMask(mask.size(), r);
        cv::Mat inside; cv::bitwise_and(mask, rMask, inside);

        double rectPix = (double)cv::countNonZero(rMask);
        double maskPix = (double)cv::countNonZero(inside);
        double purity = (rectPix>0.0) ? (maskPix/rectPix) : 0.0;
        if (purity < minPurity) continue;

        // 平均 Saturation / a / b
        cv::Scalar meanH, meanS, meanV;
        cv::Scalar meanL, meanA, meanB;
        cv::Scalar stdDummy;

        cv::meanStdDev(hsvCh[1], meanS, stdDummy, inside); // saturation
        cv::meanStdDev(labCh[1], meanA, stdDummy, inside);
        cv::meanStdDev(labCh[2], meanB, stdDummy, inside);

        cv::Moments M = cv::moments(c);
        cv::Point2f cen = (M.m00!=0.0) ? cv::Point2f((float)(M.m10/M.m00),(float)(M.m01/M.m00)) : r.center;

        Pillar p;
        p.rect = r;
        p.centroid = cen;
        p.area = a;
        p.purity = purity;
        p.aspect = aspect;
        p.meanSat = meanS[0];
        p.meanA = meanA[0]-128.0;
        p.meanB = meanB[0]-128.0;

        // 追加の “グレー確からしさ” 検査（平均 a*, b* をもう一度）
        if (std::abs(p.meanA) > aThr || std::abs(p.meanB) > bThr) continue;

        pillars.push_back(p);
    }

    if (pillars.size() < 2){
        // プレビューのみ
        cv::Mat maskColor; cv::cvtColor(mask, maskColor, cv::COLOR_GRAY2BGR);
        cv::Mat overlay = bgr.clone();
        cv::Mat side;
        cv::hconcat(std::vector<cv::Mat>{bgr, maskColor, overlay}, side);
        res.previewFile = "out_gray_gate_preview.jpg";
        cv::imwrite(res.previewFile, side);
        return res;
    }

    // 面積降順
    std::sort(pillars.begin(), pillars.end(), [](const Pillar& A, const Pillar& B){
        return A.area > B.area;
    });

    double bestScore=-1.0;
    Pillar bestL, bestR;

    for (size_t i=0;i<pillars.size();++i){
        for (size_t j=i+1;j<pillars.size();++j){
            const Pillar *A=&pillars[i], *B=&pillars[j];
            const Pillar *L = (A->centroid.x <= B->centroid.x) ? A : B;
            const Pillar *R = (L==A?B:A);
            double sep = std::abs(R->centroid.x - L->centroid.x);
            double sepNorm = sep / bgr.cols;
            if (sepNorm < minSepNorm) continue;

            double score = (A->area + B->area) * 0.5
                           * (A->purity + B->purity) * 0.5
                           * sepNorm;
            if (score > bestScore){
                bestScore = score;
                bestL = *L;
                bestR = *R;
            }
        }
    }

    if (bestScore < 0){
        // 条件を満たすペアが無い → 上位 2 本
        bestL = pillars[0];
        bestR = pillars[1];
        if (bestL.centroid.x > bestR.centroid.x) std::swap(bestL,bestR);
    }

    res.left = bestL;
    res.right = bestR;
    res.foundTwo = true;
    res.separationPx = std::abs(res.right.centroid.x - res.left.centroid.x);
    res.separationNorm = res.separationPx / bgr.cols;
    res.gateCenter = cv::Point2f(
        (res.left.centroid.x + res.right.centroid.x)*0.5f,
        (res.left.centroid.y + res.right.centroid.y)*0.5f
    );
    double imgAreaNorm = std::min(1.0, (res.left.area + res.right.area)/(0.25*imgArea));
    res.confidence = ((res.left.purity + res.right.purity)*0.5) * res.separationNorm * imgAreaNorm;

    // プレビュー
    cv::Mat maskColor; cv::cvtColor(mask, maskColor, cv::COLOR_GRAY2BGR);
    cv::Mat overlay = bgr.clone();
    auto drawPillar=[&](const Pillar& p, const cv::Scalar& col){
        cv::Point2f pts[4]; p.rect.points(pts);
        for(int k=0;k<4;k++)
            cv::line(overlay, pts[k], pts[(k+1)%4], col, 3, cv::LINE_AA);
        cv::drawMarker(overlay, p.centroid, cv::Scalar(0,0,255), cv::MARKER_CROSS, 24, 2);
        char txt[160];
        std::snprintf(txt,sizeof(txt),"A=%.0f P=%.2f Asp=%.2f S=%.1f a=%.1f b=%.1f",
                      p.area,p.purity,p.aspect,p.meanSat,p.meanA,p.meanB);
        cv::putText(overlay, txt, p.centroid + cv::Point2f(6,-10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, col, 2, cv::LINE_AA);
    };
    drawPillar(res.left,  cv::Scalar(0,200,0));
    drawPillar(res.right, cv::Scalar(0,140,255));
    cv::drawMarker(overlay, res.gateCenter, cv::Scalar(255,0,0), cv::MARKER_STAR, 30, 2);
    char gtxt[128];
    std::snprintf(gtxt,sizeof(gtxt),"Gate(%.1f,%.1f) conf=%.2f",
                  res.gateCenter.x,res.gateCenter.y,res.confidence);
    cv::putText(overlay, gtxt, res.gateCenter + cv::Point2f(10,30),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255,0,0), 2, cv::LINE_AA);

    cv::Mat side;
    cv::hconcat(std::vector<cv::Mat>{bgr, maskColor, overlay}, side);
    res.previewFile = "out_gray_gate_preview.jpg";
    cv::imwrite(res.previewFile, side);
    return res;
}

int main(int argc, char** argv){
    // 使い方:
    // ./detectGrayGateCenterExt image.jpg
    //  追加パラメータ (順番):
    // Hmin Hmax Smin Smax Vmin Vmax deltaMax aThr bThr rejectGreen(0/1)
    // greenHmin greenHmax greenSMin blurK openK closeK minAspect minPurity minSepPercent
    //
    // 例:
    // ./detectGrayGateCenterExt frame.jpg 0 179 0 55 25 235 22 10 10 1 35 95 25 3 2 2 2.0 0.35 10
    std::string filename = (argc>1)? argv[1] : "sample.jpg";
    cv::Mat src = cv::imread(filename);
    if (src.empty()){
        std::cerr << "画像読込失敗: " << filename << std::endl;
        return 1;
    }

    // デフォルト値
    int Hmin=0,Hmax=179,Smin=0,Smax=60,Vmin=30,Vmax=230;
    int deltaMax=25;     // max(R,G,B)-min(R,G,B) 閾値
    int aThr=10, bThr=10;
    int rejectGreen=1;
    int greenHmin=35, greenHmax=95, greenSMin=25;
    int blurK=1, openK=2, closeK=2;
    double minAspect=2.0;
    double minPurity=0.30;
    double minSepPercent=10.0;

    std::vector<double> nums;
    for (int i=2;i<argc;i++){ try{ nums.push_back(std::stod(argv[i])); } catch(...){} }

    // 可変引数適用（存在すれば順に）
    int idx=0;
    auto next=[&](double &v){ if (idx<(int)nums.size()) v=nums[idx++]; };
    auto nextI=[&](int &v){ if (idx<(int)nums.size()) v=(int)nums[idx++]; };

    nextI(Hmin); nextI(Hmax); nextI(Smin); nextI(Smax); nextI(Vmin); nextI(Vmax);
    nextI(deltaMax); nextI(aThr); nextI(bThr); nextI(rejectGreen);
    nextI(greenHmin); nextI(greenHmax); nextI(greenSMin);
    nextI(blurK); nextI(openK); nextI(closeK);
    { double tmp; next(tmp); if (idx<= (int)nums.size()) if (tmp!=0.0) minAspect = tmp; }
    { double tmp; next(tmp); if (idx<= (int)nums.size()) if (tmp!=0.0) minPurity = tmp; }
    { double tmp; next(tmp); if (idx<= (int)nums.size()) if (tmp!=0.0) minSepPercent = tmp; }

    auto result = detectGrayGateCenter(
        src,
        Hmin,Hmax,Smin,Smax,Vmin,Vmax,
        deltaMax,aThr,bThr,
        (rejectGreen!=0),greenHmin,greenHmax,greenSMin,
        blurK,openK,closeK,
        0.0008,0.30,
        minAspect,minPurity,(minSepPercent/100.0)
    );

    std::cout << "{"
              << "\"foundTwo\":" << (result.foundTwo?"true":"false")
              << ",\"gateCenter\":{\"x\":" << result.gateCenter.x << ",\"y\":" << result.gateCenter.y << "}"
              << ",\"separation_px\":" << result.separationPx
              << ",\"separation_norm\":" << result.separationNorm
              << ",\"confidence\":" << result.confidence
              << ",\"preview\":\"" << result.previewFile << "\"";

    if (result.foundTwo){
        auto dumpP=[&](const Pillar& p){
            std::ostringstream oss;
            oss << "{"
                << "\"cx\":" << p.centroid.x
                << ",\"cy\":" << p.centroid.y
                << ",\"area\":" << p.area
                << ",\"purity\":" << p.purity
                << ",\"aspect\":" << p.aspect
                << ",\"meanSat\":" << p.meanSat
                << ",\"meanA\":" << p.meanA
                << ",\"meanB\":" << p.meanB
                << "}";
            return oss.str();
        };
        std::cout << ",\"pillars\":[" << dumpP(result.left) << "," << dumpP(result.right) << "]";
    } else {
        std::cout << ",\"pillars\":[]";
    }
    std::cout << "}" << std::endl;
    return 0;
}
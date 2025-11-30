#include "estimateGateCenter.h"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include <numeric> // std::iota を使う場合は忘れずに


namespace gate
{

    // ====== パラメータ調整（必要に応じて微調整してください） ==========================
    static std::string kInputFile = "sample.jpg";
    static std::string kDebugImage = "out_gate_center.jpg";

    // 灰色抽出(HSV)のしきい値
    // 低彩度(S小)、かつ中間の明るさ(V)を狙う
    // 新: 影と白飛びを抑えて、灰一帯を広めに
    static int S_MAX = 90;  // 彩度はより緩め（背景の緑を外すのはLab側で担保）
    static int V_MIN = 0;  // 影(暗部)を除去
    static int V_MAX = 100; // 白床やハイライトを抑制

    // Labで「無彩色」近傍を強く縛る（8bit-Lab: a,b=128が中性）
    static int A_TOL = 10; // |a-128| <= A_TOL
    static int B_TOL = 12; // |b-128| <= B_TOL

    // 形状フィルタ（少し縦長を厳しめに、足が短い画角も拾えるよう高さ比は微調整）
    static double MIN_ASPECT = 0;     // h/w
    static double MIN_HEIGHT_FR = 0.10; // 画像高さに対する最小高さ比

    // 足帯域（下側の帯域をやや厚めに）
    static double FOOT_BAND_FR = 0.4;

    // ポール形状フィルタ
    static double MIN_AREA_RATIO = 0.0005; // 画像面積に対する最小面積
    static double MAX_AREA_RATIO = 0.4;    // 画像面積に対する最大面積
    // ============================================================================

    // HSVで灰色マスクを作る
    // HSV + Lab の AND で灰色抽出を頑健化
    static cv::Mat makeGrayMaskHSV(const cv::Mat &bgr)
    {
        // --- HSV 条件（暗すぎ/明るすぎ/高彩度を除外）---
        cv::Mat hsv;
        cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsva;
        cv::split(hsv, hsva);
        // hsva[1]=S, hsva[2]=V
        cv::Mat s_low, v_ok, v_lo, v_hi;
        cv::threshold(hsva[1], s_low, S_MAX, 255, cv::THRESH_BINARY_INV); // S <= S_MAX
        cv::threshold(hsva[2], v_lo, V_MIN - 1, 255, cv::THRESH_BINARY);  // V >= V_MIN
        cv::threshold(hsva[2], v_hi, V_MAX, 255, cv::THRESH_BINARY_INV);  // V <= V_MAX
        cv::bitwise_and(v_lo, v_hi, v_ok);
        cv::Mat mask_hsv;
        cv::bitwise_and(s_low, v_ok, mask_hsv);

        // --- Lab 条件（無彩色近傍を選択）---
        // OpenCVの8bit-Labは a,b=128 が中立。灰は a,b が128近傍に集まる。
        cv::Mat lab;
        cv::cvtColor(bgr, lab, cv::COLOR_BGR2Lab);
        std::vector<cv::Mat> laba;
        cv::split(lab, laba); // L,a,b
        cv::Mat a_abs, b_abs, a_ok, b_ok;

        // |a-128|, |b-128| を計算
        cv::Mat a_shifted, b_shifted;
        laba[1].convertTo(a_shifted, CV_16S);
        a_shifted = a_shifted - 128;
        laba[2].convertTo(b_shifted, CV_16S);
        b_shifted = b_shifted - 128;
        cv::Mat a_abs16, b_abs16;
        cv::absdiff(a_shifted, cv::Scalar(0), a_abs16);
        cv::absdiff(b_shifted, cv::Scalar(0), b_abs16);
        a_abs16.convertTo(a_abs, CV_8U); // 0..255に丸め
        b_abs16.convertTo(b_abs, CV_8U);

        cv::threshold(a_abs, a_ok, A_TOL, 255, cv::THRESH_BINARY_INV); // |a-128| <= A_TOL
        cv::threshold(b_abs, b_ok, B_TOL, 255, cv::THRESH_BINARY_INV); // |b-128| <= B_TOL

        cv::Mat mask_lab;
        cv::bitwise_and(a_ok, b_ok, mask_lab);

        // --- 合成：HSV AND Lab（双方を満たす画素だけを採用）---
        cv::Mat mask;
        cv::bitwise_and(mask_hsv, mask_lab, mask);

        // --- 形態学的処理 ---
        // 細かなノイズを除去 → 縦構造を強めて連結
        cv::Mat kOpen = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kOpen);

        // 影で分断されたポールを繋ぐため、縦長クローズを強めに
        cv::Mat kVert = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 41));
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kVert);

        return mask;
    }

    struct Pole
    {
        cv::Rect rect;
        double height;
        double width;
        double area;
        double aspect; // h/w
        cv::Point2f footCentroid{-1.0f, -1.0f};
    };

    // 足（下端帯域）の重心を計算（マスク上の白画素の重心）
    static cv::Point2f computeFootCentroid(const cv::Mat &mask, const cv::Rect &r)
    {
        cv::Rect R = r & cv::Rect(0, 0, mask.cols, mask.rows);
        if (R.empty())
            return {-1.0f, -1.0f};

        int bandH = std::max(6, (int)std::round(R.height * FOOT_BAND_FR));
        int y0 = R.y + R.height - bandH;
        if (y0 < 0)
            y0 = 0;
        cv::Rect band(R.x, y0, R.width, R.y + R.height - y0);
        band &= cv::Rect(0, 0, mask.cols, mask.rows);
        if (band.empty())
            return {-1.0f, -1.0f};

        cv::Mat roi = mask(band);
        cv::Moments m = cv::moments(roi, true);
        if (m.m00 <= 1e-3)
            return {-1.0f, -1.0f};
        float cx = (float)(m.m10 / m.m00);
        float cy = (float)(m.m01 / m.m00);

        // 画像全体座標に戻す
        return cv::Point2f(band.x + cx, band.y + cy);
    }

    static std::vector<Pole> findCandidatePoles(const cv::Mat &mask, const cv::Size &imgSize)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        double imgArea = (double)imgSize.width * imgSize.height;
        double minArea = std::max(100.0, imgArea * MIN_AREA_RATIO);
        double maxArea = imgArea * MAX_AREA_RATIO;

        std::vector<Pole> poles;
        for (auto &c : contours)
        {
            if (c.size() < 5)
                continue;
            double a = cv::contourArea(c);
            if (a < minArea || a > maxArea)
                continue;

            cv::Rect r = cv::boundingRect(c);
            double w = std::max(1, r.width);
            double h = std::max(1, r.height);
            double asp = h / w;

            if (asp < MIN_ASPECT)
                continue; // 十分に縦長か
            if (h < imgSize.height * MIN_HEIGHT_FR)
                continue; // ある程度の高さがあるか

            Pole p;
            p.rect = r;
            p.height = h;
            p.width = w;
            p.area = a;
            p.aspect = asp;
            poles.push_back(p);
        }

        // 左右位置（x）で安定して選べるようにソート
        std::sort(poles.begin(), poles.end(), [](const Pole &a, const Pole &b)
                  { return a.rect.x < b.rect.x; });
        return poles;
    }

    // 候補群から左右2本を選ぶ（基本は最も外側の2本を採用、近接や重なりを避ける）
    static std::pair<int, int> pickLeftRightIndices(const std::vector<Pole> &poles)
    {
        if (poles.size() < 2)
            return {-1, -1};

        // まずは最も左と最も右
        int leftIdx = 0;
        int rightIdx = (int)poles.size() - 1;

        // もし左右がほぼ同じ領域（重なりすぎ）なら高さのある2本を選ぶ
        auto overlappedTooMuch = [&](int i, int j)
        {
            const auto &A = poles[i].rect;
            const auto &B = poles[j].rect;
            int inter = (A & B).area();
            return inter > std::min(A.area(), B.area()) * 0.5;
        };
        if (overlappedTooMuch(leftIdx, rightIdx))
        {
            // 高さ上位2本をxで並び替えて採用
            std::vector<int> idx(poles.size());
            std::iota(idx.begin(), idx.end(), 0);
            std::sort(idx.begin(), idx.end(), [&](int a, int b)
                      {
            if (poles[a].height == poles[b].height) return poles[a].rect.x < poles[b].rect.x;
            return poles[a].height > poles[b].height; });
            if (idx.size() >= 2)
            {
                int i = idx[0], j = idx[1];
                if (poles[i].rect.x < poles[j].rect.x)
                    return {i, j};
                else
                    return {j, i};
            }
            else
            {
                return {-1, -1};
            }
        }

        return {leftIdx, rightIdx};
    }

    static void drawDebug(const cv::Mat &src, const cv::Mat &mask,
                          const FeetDetection &out, const std::vector<Pole> &poles)
    {
        // カラーマップ化したマスク
        cv::Mat maskColor;
        cv::applyColorMap(mask, maskColor, cv::COLORMAP_OCEAN);

        // オーバレイ
        cv::Mat overlay = src.clone();

        // すべての候補ポールを描画
        for (const auto &p : poles)
        {
            cv::rectangle(overlay, p.rect, cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
            // 足帯域
            int bandH = std::max(6, (int)std::round(p.rect.height * FOOT_BAND_FR));
            cv::Rect band(p.rect.x, p.rect.y + p.rect.height - bandH, p.rect.width, bandH);
            band &= cv::Rect(0, 0, overlay.cols, overlay.rows);
            cv::rectangle(overlay, band, cv::Scalar(200, 200, 50), 2, cv::LINE_AA);
        }

        // 採用された左右ポールと重心
        if (out.leftFound)
        {
            cv::rectangle(overlay, out.leftRect, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
            cv::drawMarker(overlay, out.left, cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 20, 3, cv::LINE_AA);
            char buf[128];
            std::snprintf(buf, sizeof(buf), "L(%.1f,%.1f)", out.left.x, out.left.y);
            cv::putText(overlay, buf, out.left + cv::Point2f(8, -8), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(30, 220, 30), 2, cv::LINE_AA);
        }
        else
        {
            cv::putText(overlay, "LEFT POLE NOT FOUND", cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        }

        if (out.rightFound)
        {
            cv::rectangle(overlay, out.rightRect, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
            cv::drawMarker(overlay, out.right, cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 20, 3, cv::LINE_AA);
            char buf[128];
            std::snprintf(buf, sizeof(buf), "R(%.1f,%.1f)", out.right.x, out.right.y);
            cv::putText(overlay, buf, out.right + cv::Point2f(8, -8), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(30, 220, 30), 2, cv::LINE_AA);
        }
        else
        {
            cv::putText(overlay, "RIGHT POLE NOT FOUND", cv::Point(20, 80), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        }

        // 横並び合成
        int H = src.rows;
        cv::Mat maskAdj, overlayAdj;
        cv::resize(maskColor, maskAdj, cv::Size((int)std::round(mask.cols * (H / (double)mask.rows)), H));
        cv::resize(overlay, overlayAdj, cv::Size((int)std::round(overlay.cols * (H / (double)overlay.rows)), H));

        cv::Mat side;
        cv::hconcat(std::vector<cv::Mat>{src, maskAdj, overlayAdj}, side);
        cv::imwrite(kDebugImage, side);
    }

    FeetDetection estimateGateCenter()
    {
        FeetDetection result;

        cv::Mat src = cv::imread(kInputFile);
        if (src.empty())
        {
            // 画像が読めない場合
            std::fprintf(stderr, "Failed to load image: %s\n", kInputFile.c_str());
            // デバッグ画像は出せないが、そのまま返す
            return result;
        }

            // ↓↓↓ ここにROI設定を追加 ↓↓↓
    // --- 画像上部のノイズ領域を無視するため、関心領域(ROI)を設定 ---
    // 画像の上から10%をカットする例
    int top_margin = src.rows * 0.2; 
    cv::Rect roi(0, top_margin, src.cols, src.rows - top_margin);
    src = src(roi).clone(); // ROIで画像を切り出し、コピーを作成
    // ↑↑↑ ここまで追加 ↑↑↑

        cv::Mat mask = makeGrayMaskHSV(src);
        auto poles = findCandidatePoles(mask, src.size());
        auto lr = pickLeftRightIndices(poles);

        if (lr.first >= 0)
        {
            Pole L = poles[lr.first];
            L.footCentroid = computeFootCentroid(mask, L.rect);
            if (L.footCentroid.x >= 0 && L.footCentroid.y >= 0)
            {
                result.leftFound = true;
                result.left = L.footCentroid;
                result.leftRect = L.rect;
            }
        }
        if (lr.second >= 0)
        {
            Pole R = poles[lr.second];
            R.footCentroid = computeFootCentroid(mask, R.rect);
            if (R.footCentroid.x >= 0 && R.footCentroid.y >= 0)
            {
                result.rightFound = true;
                result.right = R.footCentroid;
                result.rightRect = R.rect;
            }
        }

        // デバッグ出力
        drawDebug(src, mask, result, poles);

        // 端末にも出力（参考コード風）
        if (result.leftFound)
            std::printf("LeftFoot:  %.1f, %.1f\n", result.left.x, result.left.y);
        else
            std::printf("LeftFoot:  NOT FOUND\n");
        if (result.rightFound)
            std::printf("RightFoot: %.1f, %.1f\n", result.right.x, result.right.y);
        else
            std::printf("RightFoot: NOT FOUND\n");

        return result;
    }

    // ユーザー用API中央座標だけ欲しいとき用の軽いラッパ
    cv::Point2f estimateGateCenterPoint()
    {
        FeetDetection det = estimateGateCenter();
        if (det.leftFound && det.rightFound)
        {
            return cv::Point2f(
                (det.left.x + det.right.x) * 0.5f,
                ((det.left.y + det.right.y) * 0.5f)+216); // ROI分を補正
        }
        return cv::Point2f(-1.0f, -1.0f);
    }

} // namespace gate

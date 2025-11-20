// gate_x_detector.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

static vector<int> findTopPeaks(const Mat& prof, int nPeaks, int minDistPx, float relThresh){
    CV_Assert(prof.rows == 1 && prof.type() == CV_32F);
    int W = prof.cols;
    const float* p = prof.ptr<float>(0);

    double minv, maxv;
    minMaxLoc(prof, &minv, &maxv);
    float thr = static_cast<float>(maxv * relThresh);

    vector<pair<float,int>> cand;
    for (int x = 1; x < W-1; ++x) {
        if (p[x] >= thr && p[x] >= p[x-1] && p[x] >= p[x+1]) {
            cand.emplace_back(p[x], x);
        }
    }
    sort(cand.begin(), cand.end(), [](auto& a, auto& b){ return a.first > b.first; });

    vector<int> picked;
    for (auto& c : cand) {
        int xi = c.second;
        bool farEnough = true;
        for (int q : picked) if (abs(q - xi) < minDistPx) { farEnough = false; break; }
        if (farEnough) {
            picked.push_back(xi);
            if ((int)picked.size() == nPeaks) break;
        }
    }
    sort(picked.begin(), picked.end()); // 左右順
    return picked;
}

int main(int argc, char** argv){
    string path = (argc >= 2) ? argv[1] : "350_0.jpg";
    Mat img = imread(path);
    if (img.empty()) {
        cerr << "Failed to load: " << path << endl;
        return 1;
    }

    // --- 横軸のみで判定：列方向プロジェクション ---
    Mat gray, inv;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    bitwise_not(gray, inv); // 暗い縦線を大きくする(白背景前提)

    Mat prof32f;                 // 出力Matを用意
    reduce(inv, prof32f, 0, REDUCE_SUM, CV_32F);  // 1xW のプロファイル
    GaussianBlur(prof32f, prof32f, Size(51,1), 0, 0, BORDER_REPLICATE); // 横方向に平滑化

    // --- ピーク(脚)を検出 ---
    int W = img.cols, H = img.rows;
    int minDist = max(20, W/20);                      // 2ピークの最小距離
    vector<int> posts = findTopPeaks(prof32f, 2, minDist, 0.4f); // 相対閾値40%

    if (posts.size() < 2) {
        cerr << "ゲートの脚を2本検出できませんでした。" << endl;
        return 2;
    }
    int xL = posts[0], xR = posts[1];
    int xC = (xL + xR) / 2;

    cout << "Left post x = "  << xL << "\n";
    cout << "Right post x = " << xR << "\n";
    cout << "Gate center x = " << xC << " (px)\n";

    // （任意）9分割で何番か
    int slots = 9;
    int slotIdx = min(slots, max(1, xC * slots / W + 1));
    cout << "Estimated slot (1-" << slots << "): " << slotIdx << "\n";

    // 可視化
    Mat vis = img.clone();
    line(vis, Point(xL,0), Point(xL,H-1), Scalar(0,0,255), 2);
    line(vis, Point(xR,0), Point(xR,H-1), Scalar(0,255,0), 2);
    line(vis, Point(xC,0), Point(xC,H-1), Scalar(255,0,0), 1);
    putText(vis, "L", Point(xL+3, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,0,255), 2);
    putText(vis, "R", Point(xR+3, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,255,0), 2);
    putText(vis, "C", Point(xC+3, 60), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255,0,0), 2);
    imwrite("gate_detect_result.png", vis);
    cout << "Saved: gate_detect_result.png\n";
    return 0;
}

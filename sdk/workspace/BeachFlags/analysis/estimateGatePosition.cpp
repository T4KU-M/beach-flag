#include "estimateGatePosition.h"
#include <iostream>
#include <vector>

// コンストラクタ: オブジェクト作成時にカメラを開く
estimateGatePosition::estimateGatePosition(int cameraIndex)
{
    cap.open(cameraIndex);
    isCameraReady = cap.isOpened();
    if (!isCameraReady) {
        std::cerr << "Error: Could not open camera with index " << cameraIndex << std::endl;
    }
}

// デストラクタ: オブジェクト破棄時にカメラを解放する
estimateGatePosition::~estimateGatePosition()
{
    if (cap.isOpened()) {
        cap.release();
    }
}

// カメラの状態を返す
bool estimateGatePosition::isOpened() const
{
    return isCameraReady;
}

// ゲートを検出する（水平方向の重心のみ返す。ポールは面積閾値で判定）
cv::Point estimateGatePosition::find()
{
    if (!isCameraReady) {
        return cv::Point(-1, -1); // カメラがなければ処理しない
    }

    // 0. 内部のカメラから1フレーム読み込む
    cv::Mat frame;
    cap.read(frame);
    if (frame.empty()) {
        std::cerr << "Warning: Could not grab a frame." << std::endl;
        return cv::Point(-1, -1);
    }

    // 1. 入力画像をグレースケールに変換
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // 2. グレースケールの輝度で二値化（灰色抽出）: [20, 60]
    int lower = 20;
    int upper = 60;
    cv::Mat mask;
    cv::inRange(gray, lower, upper, mask);

    // 3. ノイズ除去（開閉処理）
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));

    // 4. 輪郭検出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // デバッグ用に元画像をコピー
    debugImage = frame.clone();

    // 面積で閾値をかけて2本のポールを検出
    double minArea = 500.0; // 必要に応じて調整
    std::vector<std::pair<double, int>> poleAreas;
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area >= minArea) {
            poleAreas.emplace_back(area, static_cast<int>(i));
        }
    }
    // 面積降順でソート
    std::sort(poleAreas.rbegin(), poleAreas.rend());

    if (poleAreas.size() >= 2) {
        int idx1 = poleAreas[0].second;
        int idx2 = poleAreas[1].second;

        cv::Rect rect1 = cv::boundingRect(contours[idx1]);
        cv::Rect rect2 = cv::boundingRect(contours[idx2]);

        int x1 = rect1.x + rect1.width / 2;
        int x2 = rect2.x + rect2.width / 2;

        // デバッグ描画
        cv::drawContours(debugImage, contours, idx1, cv::Scalar(0, 255, 0), 2);
        cv::rectangle(debugImage, rect1, cv::Scalar(255, 0, 0), 2);
        cv::drawContours(debugImage, contours, idx2, cv::Scalar(0, 255, 0), 2);
        cv::rectangle(debugImage, rect2, cv::Scalar(255, 0, 0), 2);

        int gate_center_x = (x1 + x2) / 2;
        int gate_center_y = frame.rows / 2; // y座標は画像中央（仮）

        cv::circle(debugImage, cv::Point(gate_center_x, gate_center_y), 5, cv::Scalar(0, 0, 255), -1);

        return cv::Point(gate_center_x, gate_center_y);
    }

    return cv::Point(-1, -1); // ポール2本分が見つからなければ
}

cv::Mat estimateGatePosition::getDebugImage()
{
    return debugImage;
}
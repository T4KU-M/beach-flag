#pragma once
#include <opencv2/opencv.hpp>
#include <string>

namespace target {
    struct DetectionResult {
        bool found{false};
        cv::Point2f centroid{0.0f, 0.0f};
        cv::RotatedRect ellipse;
        double contourArea{0.0};
        double ellipseArea{0.0};
        double purity{0.0};
    };



    cv::Point2f estimatelandingtarget();


}
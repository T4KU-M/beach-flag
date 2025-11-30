#include <opencv2/opencv.hpp>
#include "estimateGateCenter.h"
#include "estimateLandingTarget.h"
#include "goTarget.h"
#include <fstream>
#include <unistd.h> // usleep
#include <iostream>
#include <regex>



int startpic();
int writefile();

int main()
{

    // カメラ初期化
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "カメラが利用できません" << std::endl;
        return 1;
    }

    while(1){
        usleep(1000000); // 1秒待機 
        if(writefile()){
            break;
        }
    }

    while (1)
    {
        usleep(1000000); // 1秒待機
        if(startpic()){
            break;
        }
    }


    cv::Mat frame;

    while (true)
    {
        while (1)
        {
            cap >> frame;
            if (frame.empty())
            {
                std::cerr << "フレームが取得できません" << std::endl;
                break;
            }
            cv::Point2f gate_pixel;
            cv::Point2f target_pixel;
            gate_pixel = cv::Point2f(-1.0f, -1.0f);
            target_pixel = cv::Point2f(-1.0f, -1.0f);

            // フレームを sample.jpg に保存（estimateGateCenter がこれを読む）
            std::string filename = "sample.jpg";
            cv::imwrite(filename, frame);

            // --- ここでゲート中心座標を取得 ---
            gate_pixel = gate::estimateGateCenterPoint();

            // 既存のランディングターゲット（必要なら残す）
            target_pixel = target::estimatelandingtarget();

            // 要求された形： gate_location = <ここに関数を入れてください>;
            // 中心画素 → 角度・距離
            std::pair<double, double> gate_location(0, 0);
            if (gate_pixel.x > 0 && gate_pixel.y > 0)
            {
                gate_location = calculateAngleAndDistance(gate_pixel.x, gate_pixel.y);
            }

            std::pair<double, double> target_location(0, 0);
            if (target_pixel.x > 0 && target_pixel.y > 0)
            {
                target_location = calculateAngleAndDistance(target_pixel.x, target_pixel.y);
            }

            std::ofstream outputFile("../results.txt");
            if (outputFile.is_open())
            {
                outputFile << "Gate Location: Angle = " << gate_location.first
                           << ", Distance = " << gate_location.second << std::endl;
                outputFile << "Target Location: Angle = " << target_location.first
                           << ", Distance = " << target_location.second << std::endl;
                outputFile.close();
                std::cout << "結果を results.txt に保存しました。" << std::endl;
            }
            else
            {
                std::cerr << "ファイルを開けませんでした。" << std::endl;
            }

            usleep(1000000); // 1秒待機
        }
    }

    cap.release();
    return 0;
}

int startpic(){
    std::ifstream file("../startpic.txt");
    if (!file.is_open()){
        return false;
    }

    std::string line;
    std::regex int_regex("[-+]?[0-9]*\\.?[0-9]+");

    while (std::getline(file, line)) {
        auto begin = std::sregex_iterator(line.begin(), line.end(), int_regex);
        if (begin != std::sregex_iterator()) {
            int check = std::stoi(begin->str()); // 最初の数値だけ取得
            if(check==1){; 
            return true;
            }
        }
    }
    //std::cerr << "数値が見つかりませんでした" << std::endl;
    return false;
}

int writefile(){
	std::ofstream outputFile("../startpic.txt");
            if (outputFile.is_open())
            {
                outputFile << "0" << std::endl;
                outputFile.close();
                std::cout << "結果を results.txt に保存しました。" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "ファイルを開けませんでした。" << std::endl;
                return false;
            }
}
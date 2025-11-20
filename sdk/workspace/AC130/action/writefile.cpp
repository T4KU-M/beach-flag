#include "writefile.h"
#include <fstream>
#include <unistd.h> // usleep
#include <iostream>
// コンストラクタ
Writefile::Writefile() : Act()
{
}

// 速度と旋回量を決定する
void Writefile::determineSpeedAndSteering()
{
	
	std::ofstream outputFile("startpic.txt");
            if (outputFile.is_open())
            {
                outputFile << "1" << std::endl;
                outputFile.close();
                std::cout << "結果を results.txt に保存しました。" << std::endl;
            }
            else
            {
                std::cerr << "ファイルを開けませんでした。" << std::endl;
            }
	setSpeed(0);		// 速度をゼロにする
	setSteering(0);		// 旋回量をゼロにする

}

#include "readfile.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include "module_common.h"

static bool readFromPath(void);
// コンストラクタ
Readfile::Readfile() : Act()
{
}

// 速度と旋回量を決定する
void Readfile::determineSpeedAndSteering()
{
    readFromPath();
    printf("%f,%f,%f,%f\n", gateangle, gatedistance, targetangle, targetdistance);
	setSpeed(0);		// 速度をゼロにする
	setSteering(0);		// 旋回量をゼロにする
}



static bool readFromPath(void)
{
    std::ifstream file("results.txt");
    if (!file.is_open())
    {
        printf("ファイルが開けません\n");
        return false;
    }
    std::string line;
    std::regex number_regex("[-+]?[0-9]*\\.?[0-9]+");
    std::vector<double> values;

    while (std::getline(file, line)) {
        auto begin = std::sregex_iterator(line.begin(), line.end(), number_regex);
        auto end = std::sregex_iterator();
        for (auto i = begin; i != end; ++i) {
            values.push_back(std::stod(i->str()));
        }
    }

    if (values.size() < 4) {
        std::cerr << "数値が不足しています（4つ必要）" << std::endl;
        return false;
    }

    gateangle      = values[0];
    gatedistance   = values[1];
    targetangle    = values[2];
    targetdistance = values[3];

    return true;
}

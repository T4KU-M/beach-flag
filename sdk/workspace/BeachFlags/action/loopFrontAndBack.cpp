#include "loopFrontAndBack.h"
#include "module_common.h"       // gRobot, gCheck など
#include <cstdio>                // std::printf を使うなら必要

// コンストラクタ
LoopFrontAndBack::LoopFrontAndBack(int speed, int sonarDistance)
    : Act(), mSpeed(speed), mHosei(0), mSonarDistance(sonarDistance)
{
}

// 速度と旋回量を決定する
void LoopFrontAndBack::determineSpeedAndSteering()
{
    int plusMinus = determinePlusMinus();
    setSpeed(plusMinus * mSpeed - mHosei);
    setSteering(0);
}

// 速度は、前or後ろかを決定する
int LoopFrontAndBack::determinePlusMinus()
{   
    //int distance = gRobot.ultraSonicSensor()->get_distance();  // SPIKE API

    // センサーのポインタ取得（すでにgRobot.ultraSonicSensor()で取得済みならそれを使う）
    pup_device_t* sonar = const_cast<pup_device_t*>(gRobot.ultraSonicSensor());

    // 距離取得
    int distance = pup_ultrasonic_sensor_distance(sonar);

    // デバッグ表示（任意）
    // std::printf("distance: %d\n", distance);

    if (distance < mSonarDistance && distance >= 0)
    {
        mHosei += 1;
        return -1;
    }
    
    if (distance > mSonarDistance || distance == -1)
    {
        mHosei += 1;
        return 1;
    }

    if (distance == mSonarDistance)
    {
        gCheck = true;
        return 0;
    }

    std::printf("error_loopFrontAndBack\n");
    return 0;
}

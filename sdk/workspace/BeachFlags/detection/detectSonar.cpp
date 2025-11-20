#include "detectSonar.h"

// コンストラクタ
DetectSonar::DetectSonar(int distanceMax, int countMax)
	: mDistanceMax(distanceMax), mCountMax(countMax), mCount(0)
{
}

// 検知結果を確認する
bool DetectSonar::detect()
{
    //int distance = gRobot.sonarSensor()->getDistance();
    int distance = pup_ultrasonic_sensor_distance(gRobot.ultraSonicSensor());
	if (distance >= 1 && distance <= mDistanceMax)
        mCount++; 
    else
        mCount = 0;

    if (mCount >= mCountMax)
		return true;

	return false;
}
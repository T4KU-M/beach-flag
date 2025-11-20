#include "detectSonarAndDistance.h"

// コンストラクタ
DetectSonarAndDistance::DetectSonarAndDistance(int distanceMax, int countMax, double threTravelDistance)
	: mDistanceMax(distanceMax), mCountMax(countMax), mCount(0),
      mLocalizer(), mThreTravelDistance(threTravelDistance)
{
}

// 検知結果を確認する
bool DetectSonarAndDistance::detect()
{
    // 超音波検知
    int distance = gRobot.sonarSensor()->getDistance();
	if (distance >= 1 && distance <= mDistanceMax)
        mCount++; 
    else
        mCount = 0;

    if (mCount >= mCountMax)
    {
        printf("ソナー検知\n");
		return true;
    }
        
    
    // 距離検知
    mLocalizer.update();
    if(mLocalizer.travelDistance() > mThreTravelDistance)
    {
        printf("距離検知\n");
        return true;
    }
        

	return false;
}
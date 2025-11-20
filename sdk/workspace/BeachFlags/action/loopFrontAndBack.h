#ifndef LOOP_FRONT_AND_BACK_H_
#define LOOP_FRONT_AND_BACK_H_

#include "module_common.h"
#include "run.h"


// 超音波センサが特定の値を取るまで前後に動作するクラス
class LoopFrontAndBack : public Act
{
public:
	LoopFrontAndBack(int speed, int sonarDistance);

private:
	void determineSpeedAndSteering() override;
    int determinePlusMinus();
    
    int mSpeed, mHosei;
    int mSonarDistance;
};

#endif //LOOP_FRONT_AND_BACK_H_
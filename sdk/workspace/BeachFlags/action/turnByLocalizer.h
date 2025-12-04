#ifndef TURNBYLOCALIZER_H_
#define TURNBYLOCALIZER_H_

#include "module_common.h"
#include "act.h"
#include "localizer.h"
#include "calculateAngleForTurn.h"  
#include "detectAngle.h"

// 旋回クラス
class TurnByLocalizer : public Act
{

public:
	TurnByLocalizer(double targetX, double targetY, int TurningAmountForBeachFlag, Localizer &localizer, CalculateAngleForTurn &calculateAngleForTurn);


protected:
	void determineSpeedAndSteering() override;

private:
	double mTargetX;
    double mTargetY;
    int mFixedTurningAmount;
    Localizer& mLocalizer;
    CalculateAngleForTurn& mCalculateAngleForTurn; 
    bool mReset;
    int sign;
    int mCount;
};

#endif

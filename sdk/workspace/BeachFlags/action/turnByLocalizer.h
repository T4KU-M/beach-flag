#ifndef TURNBYLOCALIZER_H_
#define TURNBYLOCALIZER_H_

#include "module_common.h"
#include "act.h"
#include "localizer.h"

// 旋回クラス
class TurnByLocalizer : public Act
{

public:
	TurnByLocalizer(double targetX, double targetY, int fixedTurningAmount, Localizer &localizer);


protected:
	void determineSteering() override;

private:
	double mTargetX;
    double mTargetY;
    int mFixedTurningAmount;
    Localizer& mLocalizer;
    bool mReset;
    int sign;
};

#endif

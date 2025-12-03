#ifndef TURN_H_
#define TURN_H_

#include "module_common.h"
#include "run.h"

// 旋回クラス
class Turn : public Run
{

public:
	Turn(int fixedTurningAmount, int speedMin, int speedMax, int kp);

protected:
	void determineSteering() override;

private:
	const int mFixedTurningAmount;
	int mSpeedMin;
	bool mReset;
	int kp;
};

#endif

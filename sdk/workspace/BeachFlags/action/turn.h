#ifndef TURN_H_
#define TURN_H_

#include "module_common.h"
#include "run.h"

// 旋回クラス
class Turn : public Run
{

public:
	Turn(int fixedTurningAmount, int speedMin, int speedMax, double kp);

protected:
	void determineSteering() override;
	void setSpeed(int value) override;

private:
	const int mFixedTurningAmount;
	int mSpeedMin;
	bool mReset;
	double kp;
	int cycle_count;
};

#endif

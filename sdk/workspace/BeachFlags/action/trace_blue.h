#ifndef TRACE_BULUE_H_
#define TRACE_BULUE_H_

#include "module_common.h"
#include "run.h"

#define NO_STEERING_LOWER_LIMIT (-100)
#define NO_STEERING_UPPER_LIMIT (100)

// ライントレースクラス
class Trace_blue : public Run
{
public:
	Trace_blue(LeftOrRight lineEdge, double Kp, double Ki, double Kd, int speedMin, int speedMax);
	Trace_blue(LeftOrRight lineEdge, double Kp, double Ki, double Kd,
		  int steeringMin, int steeringMax, int speedMin, int speedMax);
	Trace_blue(LeftOrRight lineEdge, double Kp, double Ki, double Kd,
		  int steeringMin, int steeringMax, int speedMin, int speedMax, int steeringOffset);

protected:
	void determineSteering() override;

private:
	LeftOrRight mLineEdge;
	double mKp, mKi, mKd;
	double mError[4]; // 誤差履歴（0:最新, 3:過去）
	double mIntegral; // 積分項の蓄積値
	int mSteeringMin;
	int mSteeringMax;
	int mSteeringOffset;
};

#endif // TRACE_H_

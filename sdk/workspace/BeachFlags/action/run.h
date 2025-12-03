#ifndef RUN_H_
#define RUN_H_

#include "module_common.h"
#include "act.h"

// 自動速度走行クラス
class Run : public Act
{
public:
	Run(int speedMin, int speedMax);
	Run(int speedMin, int speedMax, int kp);

protected:
	void determineSpeedAndSteering() override;
	virtual void determineSteering();
	int brightness() const;

private:
	void determineSpeed();
	void calculateBrightnessVariance();

	int mSpeedMin, mSpeedMax;
	int mBrightness;
	static double mEMAvg, mEMVar;
	int cycle_count;
	int currentSpeed;
	int kp;
};

#endif

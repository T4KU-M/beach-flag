#ifndef ACT_H_
#define ACT_H_

#include "module_common.h"

// <<抽象>> 走行クラス
class Act
{
public:
	Act();
	virtual ~Act();
	void excute();
	void driveMotors();

protected:
	virtual void determineSpeedAndSteering();
	void setSpeed(int value);
	void setSteering(int value);

private:
	static int mSpeed, mSteering;
};

#endif

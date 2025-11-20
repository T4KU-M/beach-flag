#ifndef LOCALIZER_H_
#define LOCALIZER_H_

#include "module_common.h"

// 自己位置推定クラス
class Localizer
{
public:
	Localizer();
	void update();
	void coordinates(double &x, double &y, double &theta) const;
	double travelDistance() const;

private:
	double mX, mY, mTheta, mTravelDistance;
	int mLeftEncoder[2], mRightEncoder[2];
	bool mReset;
};

#endif

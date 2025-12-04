#ifndef _CALCLATEANGLE_FOR_TURN_H_
#define _CALCLATEANGLE_FOR_TURN_H_

#include "module_common.h"


// 自己位置推定クラス
class CalculateAngleForTurn
{
public:
	CalculateAngleForTurn();

	void update();
	void getangle(double &theta) const;


private:
	double mTheta;
	bool mReset;
};

#endif
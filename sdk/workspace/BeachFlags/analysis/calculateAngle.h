#ifndef _CALCLATEANGLE_H_
#define _CALCLATEANGLE_H_

#include "module_common.h"


// 自己位置推定クラス
class CalculateAngle
{
public:
	CalculateAngle();

	void update();
	void getangle(double &theta) const;


private:
	double mTheta;
	bool mReset;
};

#endif
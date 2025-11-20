#ifndef HSVMEASURE_H_
#define HSVMEASURE_H_

#include "module_common.h"

// HSV測定クラス
class HsvMeasure
{
public:
	HsvMeasure();
	void getHSV(int &h, int &s, int &v);

private:
	void ConvertRGBtoHSV();

	int mH, mS, mV;
};

#endif

#ifndef DETECT_Read_H_
#define DETECT_Read_H_

#include "module_common.h"
#include "detect.h"

class DetectRead : public Detect
{
public:
		DetectRead();
		bool detect() override;

private:
    int mTemp;
	int mCount;

};

#endif 
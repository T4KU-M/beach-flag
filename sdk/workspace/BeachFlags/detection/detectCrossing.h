#ifndef DETECT_CROSSING_H_
#define DETECT_CROSSING_H_

#include "module_common.h"
#include "detect.h"

// 交差部検知クラス
class DetectCrossing : public Detect
{
public:
	DetectCrossing();
	bool detect() override;

private:
	int mState;
};

#endif

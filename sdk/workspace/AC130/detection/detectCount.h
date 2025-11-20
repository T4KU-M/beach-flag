#ifndef DETECT_COUNT_H_
#define DETECT_COUNT_H_

#include "module_common.h"
#include "detect.h"

class DetectCount : public Detect
{
public:
		DetectCount(int maxCount);
		bool detect() override;

private:
    int mMaxCount, mCount;
};

#endif //DETECT_COUNT_H_
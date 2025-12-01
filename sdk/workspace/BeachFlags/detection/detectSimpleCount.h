#ifndef DETECT_SIMPLE_COUNT_H_
#define DETECT_SIMPLE_COUNT_H_

#include "module_common.h"
#include "detect.h"

class DetectSimpleCount : public Detect
{
public:
		DetectSimpleCount(int maxCount);
		bool detect() override;

private:
    int mMaxCount, mCount;
};

#endif //DETECT_COUNT_H_
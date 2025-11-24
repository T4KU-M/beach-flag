#ifndef DETECT_START_H_
#define DETECT_START_H_

#include "module_common.h"
#include "detect.h"

class DetectStart : public Detect
{
public:
	DetectStart();
	bool detectForBeachFlags();
	bool detect() override;

private:
	int mCount;
};

#endif // DETECT_START_H_

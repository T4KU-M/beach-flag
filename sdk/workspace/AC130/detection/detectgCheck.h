#ifndef DETECT_GCHECK_H_
#define DETECT_GCHECK_H_

#include "module_common.h"
#include "detect.h"

// gCheckを返すだけのクラス
class DetectgCheck : public Detect
{
public:
	DetectgCheck();
	bool detect() override;

private:
    //なし
};

#endif //DETECT_GCHECK_H_
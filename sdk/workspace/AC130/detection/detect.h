#ifndef DETECT_H_
#define DETECT_H_

#include "module_common.h"

// <<抽象>> 検知クラス
class Detect
{
public:
	Detect();
	virtual ~Detect();
	virtual bool detect();
};

#endif

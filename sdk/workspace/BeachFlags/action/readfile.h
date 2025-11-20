#ifndef _READFILE_H_
#define _READFILE_H_

#include "module_common.h"
#include "act.h"

// ファイル読み取りたいクラス
class Readfile : public Act
{
public:
	Readfile();

protected:
	void determineSpeedAndSteering() override;
};

#endif

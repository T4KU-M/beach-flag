#ifndef WRITEFILE_H_
#define WRITEFILE_H_

#include "module_common.h"
#include "act.h"

// アイドリング（停止）クラス
class Writefile : public Act
{
public:
	Writefile();

protected:
	void determineSpeedAndSteering() override;
};

#endif

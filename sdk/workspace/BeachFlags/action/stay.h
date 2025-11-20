#ifndef STAY_H_
#define STAY_H_

#include "module_common.h"
#include "act.h"

// アイドリング（停止）クラス
class Stay : public Act
{
public:
	Stay();

protected:
	void determineSpeedAndSteering() override;
};

#endif

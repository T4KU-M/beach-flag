#ifndef RESET_ANGLE_SUM_H_
#define RESET_ANGLE_SUM_H_

#include "module_common.h"
#include "act.h"

// アイドリング（停止）クラス
class resetAngleSum : public Act
{
public:
    resetAngleSum();

protected:
    void determineSpeedAndSteering() override;
};

#endif

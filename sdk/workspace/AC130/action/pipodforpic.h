#ifndef PIPODFORPIC_H
#define PIPODFORPIC_H

#include "module_common.h"
#include "act.h"

// 
class Pipodforpic : public Act
{
public:
	Pipodforpic(Target target);

protected:
	void determineSpeedAndSteering() override;
private:
    // 📣 .cpp で使うメンバー変数を宣言
    Target mTarget;
    bool mReset;
    
};

#endif

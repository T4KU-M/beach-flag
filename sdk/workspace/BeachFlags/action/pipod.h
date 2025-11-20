#ifndef PIPOD_H
#define PIPOD_H

#include "module_common.h"
#include "act.h"

// 
class Pipod : public Act
{
public:
	Pipod(LeftOrRight LineEdge);

protected:
	void determineSpeedAndSteering() override;
private:
    // 📣 .cpp で使うメンバー変数を宣言
    LeftOrRight mLineEdge;
    bool mReset;
    
};

#endif

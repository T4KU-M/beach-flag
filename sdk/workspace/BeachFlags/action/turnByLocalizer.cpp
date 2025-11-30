#include "turnByLocalizer.h"
//#include "Motor.h"
#include <motor.h>
//#include "MotorPair.h"
#include "module_common.h"

// コンストラクタ
TurnByLocalizer::TurnByLocalizer(double targetX, double targetY, int fixedTurningAmount, Localizer &localizer)
	: mTargetX(targetX), mTargetY(targetY), mFixedTurningAmount(fixedTurningAmount), mLocalizer(localizer)
{
    setSpeed(0); // 速度0に設定
	mReset = true;
    sign = 0;
}

// 速度と旋回量を決定する(override)
void TurnByLocalizer::determineSpeedAndSteering()
{
    // 便宜上ここに実装するが、スピードは0固定
    // 旋回量の絶対値も固定にしちゃう

    // 最初のみLocalizerより自己位置を取得
    if (mReset)
    {
        // 現在位置をグローバル変数に格納
        mLocalizer.update();
        mLocalizer.coordinates(currentX, currentY, currentTheta);

        // 目標回転角度を計算し、detect系クラスから参照できるようにグローバル変数に格納する 
        double deltaX = mTargetX - currentX;
        double deltaY = mTargetY - currentY;
        currentTargetTheta = (-1 * currentTheta) + std::atan2(deltaY, deltaX); // radian

        // 旋回方向を決定
        if (currentTargetTheta > 0)
        {
            sign = 1; // 反時計回り
        }
        else
        {
            sign = -1; // 時計回り
        }

        mReset = false;
    }

    // detect系クラスが終了検知するまで、一定旋回量をセットし続ける
    setSteering(sign * mFixedTurningAmount);
}

#include "turnByLocalizer.h"
//#include "Motor.h"
#include <motor.h>
//#include "MotorPair.h"
#include "module_common.h"
#include <cmath>

// コンストラクタ
TurnByLocalizer::TurnByLocalizer(double targetX, double targetY, int TurningAmountForBeachFlag, Localizer &localizer, CalculateAngleForTurn &calculateAngleForTurn)
	: mTargetX(targetX), mTargetY(targetY), mFixedTurningAmount(TurningAmountForBeachFlag), mLocalizer(localizer), mCalculateAngleForTurn(calculateAngleForTurn)
{
    // setSpeed(0); // 速度0に設定
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
        // 現在位置(x, y)をグローバル変数に格納
        double dummyTheta; // こいつは使わない
        mLocalizer.update();
        mLocalizer.coordinates(currentX, currentY, dummyTheta);

        // 現在角度(theta)をグローバル変数に格納
        mCalculateAngleForTurn.update();
        mCalculateAngleForTurn.getangle(currentTheta);
        currentTheta = currentTheta * M_PI / 180; // radian変換 

        // log
        printf("TurnByLocalizer::determineSpeedAndSteering() : currentX = %f, currentY = %f, currentTheta = %f\n", currentX, currentY, currentTheta);

        // 目標回転角度を計算し、detect系クラスから参照できるようにグローバル変数に格納する 
        double deltaX = mTargetX - currentX;
        double deltaY = mTargetY - currentY;
        // currentTargetTheta = (-1 * currentTheta) + std::atan2(deltaY, deltaX); // radian
        currentTargetTheta = (-1 * currentTheta) + std::atan(deltaY / deltaX); // radian

        int currentTargetThetaDeg = ((int)(currentTargetTheta * 180 / 3.1415)) % 360; // degree
        if(currentTargetThetaDeg > 180){
            currentTargetThetaDeg -= 360;
        }
        // else if(currentTargetThetaDeg < -180){
        //     currentTargetThetaDeg += 360;
        // }

        // log
        printf("TurnByLocalizer::determineSpeedAndSteering() : deltaX = %f, deltaY = %f, currentTargetTheta = %f, currentTargetThetaDeg = %f\n", deltaX, deltaY, currentTargetTheta, currentTargetThetaDeg);

        // 旋回方向を決定
        if (currentTargetThetaDeg > 0)
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
    setSpeed(0); // 速度0に設定
}

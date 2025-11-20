#include "act.h"
#include "module_common.h"  // gRobot などのグローバル変数が宣言されている前提

int Act::mSpeed;
int Act::mSteering;

// コンストラクタ
Act::Act()
{
}

// デストラクタ
Act::~Act()
{
}

// 走行を実行する
void Act::excute()
{
    determineSpeedAndSteering();
    driveMotors();
}

// 速度と旋回量を決定する
void Act::determineSpeedAndSteering()
{
    // ここに必要なら制御アルゴリズムを実装
}

// モータを駆動する
void Act::driveMotors()
{
    int LPWM, RPWM;
    if (mSpeed != 0)
    {
        LPWM = mSpeed - (mSteering * mSpeed) / 100;
        RPWM = mSpeed + (mSteering * mSpeed) / 100;
    }
    else
    {
        LPWM = -mSteering;
        RPWM = +mSteering;
    }

    // SPIKEでは set_power() を使う
    //gRobot.leftMotor()->pup_motor_set_power(LPWM);
    //gRobot.rightMotor()->pup_motor_set_power(RPWM);
    pup_motor_set_power(gRobot.leftMotor(), LPWM);
    pup_motor_set_power(gRobot.rightMotor(), RPWM);

}

// 速度を設定する
void Act::setSpeed(int value)
{
    // 値を -100 ～ 100 の範囲に制限
    value = (value < -100) ? -100 : ((value > 100) ? 100 : value);

    // 加速は台形制御で制限（+1）
    int sign = (value >= 0) ? 1 : -1;
    int norm = std::abs(value);
    int limit = std::abs(mSpeed) + 1;
    norm = (norm > limit) ? limit : norm;
    mSpeed = sign * norm;
}

// 旋回量を設定する
void Act::setSteering(int value)
{
    // -100 ～ 100 に制限
    value = (value < -100) ? -100 : ((value > 100) ? 100 : value);
    mSteering = value;
}

#include "run.h"
#include "module_common.h"        // gRobot
#include <cmath>                  // std::sqrt, std::pow
#include <spike/pup/colorsensor.h>
#include <stdio.h>
#include <algorithm> //ChatGPT

double Run::mEMAvg;
double Run::mEMVar;

// コンストラクタ
Run::Run(int speedMin, int speedMax)
    : Act(), mSpeedMin(speedMin), mSpeedMax(speedMax), mBrightness(), cycle_count(0), currentSpeed(0), kp(1)
{
}
Run::Run(int speedMin, int speedMax, int kp)
    : Act(), mSpeedMin(speedMin), mSpeedMax(speedMax), mBrightness(), cycle_count(0), currentSpeed(0), kp(kp)
{
}

// 速度と旋回量を決定する
void Run::determineSpeedAndSteering()
{
    determineSpeed();
    determineSteering();  // 中身は空 or 派生クラスでオーバーライド
}

// 反射光の強度を取得する
int Run::brightness() const
{
    return mBrightness;
}

// 旋回量を決定する（オーバーライドまたは継承で使用）
void Run::determineSteering()
{
    // 派生クラスでオーバーライドしてください
}

// 速度を決定する
void Run::determineSpeed()
{
    // // SPIKEのget_color_rgbを使ってRGB値を取得
    // auto rgb = pup_color_sensor_rgb(gRobot.colorSensor());

    // // 明るさの定義（最も低い値を使用＝黒に近い）
    // // 簡単で安全な方法
    // mBrightness = std::min(std::min(rgb.r, rgb.g), rgb.b);


    // // 反射光の強度の移動分散を計算する
    // calculateBrightnessVariance();

    // // 速度を決定（分散に応じて減速）
    // int speed = mSpeedMax - 0.25 * std::sqrt(mEMVar);
    // if (speed < mSpeedMin) speed = mSpeedMin;

    // // log
    // printf("Run::determineSpeed() brightness: %d, EMVar: %f, speed: %d\n", mBrightness, mEMVar, speed);

    this->cycle_count = this->cycle_count + 1; // カウントアップ
    int kp = this->kp;
    if (this->cycle_count % 20 == 0) // 200msごとに速度更新(わんちゃん周期違う説あるんですけども。あと吉田さん曰く、200msの変化はいい感じに人間が認識できるらしい)
    {
        this->currentSpeed = this->currentSpeed + kp;
    }
    if (this->currentSpeed > this->mSpeedMax)
    {
        this->currentSpeed = this->mSpeedMax;
    }
    if (this->currentSpeed < this->mSpeedMin) // まあ、これにはならんけど一応書いとくわ
    {
        this->currentSpeed = this->mSpeedMin;
    }
    printf("Run::determineSpeed() currentSpeed: %d, cycle_count: %d, kp: %d, speedmin: %d, speedmax: %d\n", this->currentSpeed, this->cycle_count, this->kp, this->mSpeedMin, this->mSpeedMax);

    setSpeed(this->currentSpeed);
}

// 反射光の強度の移動分散を計算する
void Run::calculateBrightnessVariance()
{
    const double tau_x10ms = 10.0;
    double delta = mBrightness - mEMAvg;
    double a = 1.0 - std::exp(-1.0 / tau_x10ms);
    mEMAvg = mEMAvg + a * delta;
    mEMVar = (1.0 - a) * (mEMVar + a * std::pow(delta, 2));
}

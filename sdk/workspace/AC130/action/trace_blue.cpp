#include "trace_blue.h"
#include "module_common.h"
#include <algorithm> // std::min, std::max
#include <cmath>     // std::log, std::pow

// 補正関数：明るさを位置的に補正
static double hosei(double br);

// コンストラクタ（オーバーロード）
Trace_blue::Trace_blue(LeftOrRight lineEdge, double Kp, double Ki, double Kd, int speedMin, int speedMax)
    : Run(speedMin, speedMax),
      mLineEdge(lineEdge), mKp(Kp), mKi(Ki), mKd(Kd), mError(),
      mSteeringMin(-100), mSteeringMax(100), mSteeringOffset(0),
      mIntegral(0.0) // ← 積分初期化
{
}

Trace_blue::Trace_blue(LeftOrRight lineEdge, double Kp, double Ki, double Kd, int steeringMin, int steeringMax, int speedMin, int speedMax)
    : Run(speedMin, speedMax),
      mLineEdge(lineEdge), mKp(Kp), mKi(Ki), mKd(Kd), mError(),
      mSteeringMin(steeringMin), mSteeringMax(steeringMax), mSteeringOffset(0),
      mIntegral(0.0) // ← 積分初期化
{
}

Trace_blue::Trace_blue(LeftOrRight lineEdge, double Kp, double Ki, double Kd, int steeringMin, int steeringMax, int speedMin, int speedMax, int steeringOffset)
    : Run(speedMin, speedMax),
      mLineEdge(lineEdge), mKp(Kp), mKi(Ki), mKd(Kd), mError(),
      mSteeringMin(steeringMin), mSteeringMax(steeringMax), mSteeringOffset(steeringOffset),
      mIntegral(0.0) // ← 積分初期化
{
}

// 旋回量を決定（PID制御：条件付き積分）
void Trace_blue::determineSteering()
{
    double u = 0.0, dt = 0.01;
    double center = (gWhite + 400) / 2.0;

    pup_color_rgb_t rgb = pup_color_sensor_rgb(gRobot.colorSensor());
    int brightness = rgb.r;

    // 範囲クリップ（EV3と同様の振る舞い）
    brightness = std::max(gBlack, std::min(gWhite, brightness));

    // 誤差履歴をシフト
    for (int i = 3; i > 0; i--)
    {
        mError[i] = mError[i - 1];
    }
    mError[0] = hosei((brightness - center)*(5/3));

    // 誤差・微分
    const double e = mError[0];
    const double de = (mError[0] - mError[3]) / (3.0 * dt);

    // まずPDを計算（符号反転は後段でまとめて適用）
    const double u_pd = mKp * e + mKd * de;

    // 操舵の物理上限をu空間で表現（オフセットを除いた範囲）
    const int u_min = mSteeringMin - mSteeringOffset;
    const int u_max = mSteeringMax - mSteeringOffset;

    // ラインの左右で操舵符号が反転するので、判定は符号反転を織り込んだuで行う
    const int sign = (mLineEdge == Left) ? -1 : 1;

    // --- 条件付き積分（アンチワインドアップ） ---
    // 次のステップで積分した場合の「仮の」u（符号反転込み）を作って飽和判定
    const double di = e * dt; // 積分の増分
    const double u_next_unsat = sign * (u_pd + mKi * (mIntegral + di));

    bool allow_integrate = false;
    if (u_next_unsat >= u_min && u_next_unsat <= u_max)
    {
        // 飽和しない → 積分OK
        allow_integrate = true;
    }
    else if (u_next_unsat > u_max)
    {
        // 上側に飽和しそう → uを下げる方向（sign*e < 0）のときだけ積分OK
        allow_integrate = (sign * e < 0);
    }
    else // u_next_unsat < u_min
    {
        // 下側に飽和しそう → uを上げる方向（sign*e > 0）のときだけ積分OK
        allow_integrate = (sign * e > 0);
    }

    if (allow_integrate)
    {
        mIntegral += di;
        // （任意）ごく緩いソフトクリップでドリフト抑制
        const double i_lim = 100.0; // 要調整
        if (mIntegral > i_lim)
            mIntegral = i_lim;
        if (mIntegral < -i_lim)
            mIntegral = -i_lim;
    }

    // 最終u（符号反転をここでまとめて適用）
    u = sign * (u_pd + mKi * mIntegral);

    // 整数化・クリップ
    int steering = mSteeringOffset + static_cast<int>(u);
    steering = std::max(mSteeringMin, std::min(mSteeringMax, steering));

    setSteering(steering);
}

// 線形補正関数（補間による非線形変換）
static double hosei(double br)
{
    const double a = 5.02897, b = 13.7842;
    constexpr int size = 101;
    static double x[size], y[size];
    static bool mk = true;

    if (mk)
    {
        for (int i = 0; i < size; i++)
        {
            auto f = [](double x, double n)
            {
                return std::log((std::pow(n, -x - 1) + std::pow(n, x + 1)) /
                                (std::pow(n, -x + 1) + std::pow(n, x - 1))) /
                       std::log(n);
            };
            x[i] = -10.0 + 20.0 * ((double)i / (size - 1));
            y[i] = f(x[i] / a, b) * (gWhite - gBlack) / 4.0;
        }
        mk = false;
    }

    br = std::max(y[0], std::min(y[size - 1], br));
    for (int i = 0; i < size - 1; i++)
    {
        if (br >= y[i] && br <= y[i + 1])
        {
            double x_ = (y[i + 1] - br) / (y[i + 1] - y[i]) * x[i] + (br - y[i]) / (y[i + 1] - y[i]) * x[i + 1];
            return 6.5 * x_;
        }
    }
    return 0.0;
}

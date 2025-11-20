#include "robot.h"

// 動作確認20250716
// ただし、ヘッダーのパスは変える必要あり

Robot::Robot()
{
    mColorSensor = pup_color_sensor_get_device(PBIO_PORT_ID_E);
    multraSonicSensor = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_F);
    mForceSensor = pup_force_sensor_get_device(PBIO_PORT_ID_D);
    mLeftMotor = pup_motor_get_device(PBIO_PORT_ID_B);
    mRightMotor = pup_motor_get_device(PBIO_PORT_ID_A);
}

Robot::~Robot()
{
    // SPIKE-RTでは明示的な破棄関数は不要
    // ただしシャットダウンやデタッチ処理が必要な場合はここに記述
}

pup_device_t *Robot::colorSensor()
{
    return mColorSensor;
}

pup_device_t *Robot::ultraSonicSensor()
{
    return multraSonicSensor;
}

pup_device_t *Robot::forceSensor()
{
    return mForceSensor;
}

pup_motor_t *Robot::leftMotor()
{
    return mLeftMotor;
}

pup_motor_t *Robot::rightMotor()
{
    return mRightMotor;
}
/*
pup_motor_t *Robot::leftMotor()
{
    return mLeftMotor;
}

pup_motor_t *Robot::rightMotor()
{
    return mRightMotor;
}
*/
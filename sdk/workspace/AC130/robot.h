#ifndef ROBOT_H_
#define ROBOT_H_

#include <colorsensor.h>
#include <ultrasonicsensor.h>
#include <forcesensor.h>
#include <motor.h>
#include <pbio/port.h>

//動作確認20250716
//ただし、ヘッダーのパスは変える必要あり

class Robot {
public:
    Robot();
    ~Robot();

    //const pup_device_t *colorSensor() const;
    pup_device_t *colorSensor();
    //const pup_device_t *sonarSensor() const;
    pup_device_t *ultraSonicSensor();
    //const pup_device_t *forceSensor() const;
    pup_device_t *forceSensor();
    //const pup_motor_t *leftMotor() const;
    //const pup_motor_t *rightMotor() const;

    pup_motor_t *leftMotor();
    pup_motor_t *rightMotor();

private:
    pup_device_t *mColorSensor;
    pup_device_t *multraSonicSensor;
    pup_device_t *mForceSensor;
    pup_motor_t *mLeftMotor;
    pup_motor_t *mRightMotor;
};

#endif

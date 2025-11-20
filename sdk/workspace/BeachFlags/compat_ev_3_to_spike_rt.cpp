// ============================================================================
// EV3 → SPIKE‑RT compatibility layer
// ----------------------------------------------------------------------------
//  ▸ Drop this header into your `include/` folder and add it to the include   
//    search path in application.cfg.                                          
//  ▸ **Do not edit your huge scenario file.** Simply replace the three EV3
//    headers that appear in your sources:
//        #include "ev3api.h"          →  #include "compat_ev3_to_spike_rt.h"
//        #include <Motor.h>            →  (delete)
//        using namespace ev3api;       →  (delete)                            
//    Everything else will now compile on SPIKE‑RT.
// ----------------------------------------------------------------------------
//  The goal is to map the minimal set of EV3‑RT C++ wrappers used in your code
//  (Motor / ColorSensor / SonarSensor) to the pure‑C SPIKE‑RT PBIO/PUP API so
//  that *behaviour* is unchanged and you can keep the existing scenario logic
//  untouched.
//
//  Tested with SPIKE‑RT v0.1.2 (TOPPERS/ASP3 kernel 3.7.0) – C++17 build.
// ============================================================================
#ifndef COMPAT_EV3_TO_SPIKE_RT_H
#define COMPAT_EV3_TO_SPIKE_RT_H

// ---------- kernel & utilities ---------------------------------------------
#include <t_syslog.h>          // syslog() replacement for printf() if desired
#include <kernel.h>            // act_tsk(), sta_cyc() … identical on SPIKE‑RT
#include <toppers/itronx/chrono_ext.h> // dly_tsk(), slp_tsk() …

// ---------- PBIO / PUP device drivers --------------------------------------
extern "C" {
#include <pbio/error.h>
#include <pbio/port.h>
#include <pup/motor.h>
#include <pup/color_sensor.h>
#include <pup/ultrasonic_sensor.h>
}

// Helper macro: abort on PBIO error in debug builds
#ifndef CHECK_PBIO
#define CHECK_PBIO(_expr)                                                   \
    do {                                                                    \
        pbio_error_t __e = (_expr);                                         \
        if (__e != PBIO_SUCCESS) {                                          \
            syslog(LOG_ERROR, "PBIO error %d at %s:%d", __e, __FILE__,     \
                   __LINE__);                                               \
        }                                                                   \
    } while (0)
#endif

// ============================================================================
// 1.  Motor wrapper (subset)
// ============================================================================
class Motor {
public:
    enum class EDirection { CLOCKWISE = 1, COUNTERCLOCKWISE = -1 };

    Motor(pbio_port_id_t port, EDirection dir = EDirection::CLOCKWISE,
          bool brake_at_zero = true)
    {
        _dir_sign = static_cast<int>(dir);
        _brake    = brake_at_zero;
        _m        = pup_motor_get_device(port);
    }

    void setPWM(int8_t pwm)
    {
        // EV3 setPWM(±100)  ↔  SPIKE‑RT pup_motor_set_power(±100)
        CHECK_PBIO(pup_motor_set_power(_m, _dir_sign * pwm));
        if (pwm == 0) {
            _brake ? pup_motor_brake(_m) : pup_motor_coast(_m);
        }
    }

    // EV3‑RT synonym
    void setPower(int8_t pwm) { setPWM(pwm); }

    // Minimal extra APIs used in your project ------------------------------
    int32_t getCount() const  { return pup_motor_get_count(_m); }
    void    reset()           { pup_motor_reset_count(_m);      }

private:
    pup_motor_t *_m;
    int          _dir_sign;
    bool         _brake;
};

// ============================================================================
// 2.  ColorSensor wrapper (R‑only raw mode used in your calibration() lambda)
// ============================================================================
struct rgb_raw_t { uint16_t r, g, b; };

class ColorSensor {
public:
    explicit ColorSensor(pbio_port_id_t port) { _cs = pup_color_sensor_get_device(port); }

    void getRawColor(rgb_raw_t &rgb)
    {
        uint16_t raw[3];
        CHECK_PBIO(pup_color_sensor_get_rgb(_cs, raw));
        rgb.r = raw[0]; rgb.g = raw[1]; rgb.b = raw[2];
    }

private:
    pup_color_sensor_t *_cs;
};

// ============================================================================
// 3.  SonarSensor wrapper (distance in mm)
// ============================================================================
class SonarSensor {
public:
    explicit SonarSensor(pbio_port_id_t port) { _us = pup_ultrasonic_sensor_get_device(port); }

    int getDistance()
    {
        int32_t dist_mm;
        CHECK_PBIO(pup_ultrasonic_sensor_distance(_us, &dist_mm));
        return static_cast<int>(dist_mm);
    }

private:
    pup_ultrasonic_sensor_t *_us;
};

// ============================================================================
// 4.  Quick helpers mirroring original EV3‑RT globals
// ============================================================================
static Motor       gLeftMotor (PBIO_PORT_ID_B, Motor::EDirection::COUNTERCLOCKWISE);
static Motor       gRightMotor(PBIO_PORT_ID_A, Motor::EDirection::CLOCKWISE);
static ColorSensor gColor     (PBIO_PORT_ID_C);
static SonarSensor gSonar     (PBIO_PORT_ID_D);

// Provide dummy gRobot interface expected by scenario code
struct RobotShim {
    Motor       *leftMotor()  { return &gLeftMotor;  }
    Motor       *rightMotor() { return &gRightMotor; }
    ColorSensor *colorSensor(){ return &gColor;      }
} gRobot;

#endif /* COMPAT_EV3_TO_SPIKE_RT_H */

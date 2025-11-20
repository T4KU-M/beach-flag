#ifndef DETECT_CAMERA_H_
#define DETECT_CAMERA_H_

#include "module_common.h"
#include "detect.h"

class DetectCamera : public Detect
{
public:
    DetectCamera();
    bool detect() override;

private:
    int mCount;
};

#endif // DETECT_CAMERA_H_
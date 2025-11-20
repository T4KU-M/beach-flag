#ifndef DETECT_GATE_H_
#define DETECT_GATE_H_

#include "module_common.h"
#include "detect.h"

class DetectGate : public Detect
{
public:
    DetectGate();
    bool detect() override;

private:
    int mCount;
};

#endif // DETECT_GATE_H_
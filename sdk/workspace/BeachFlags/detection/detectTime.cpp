#include "detectTime.h"

// コンストラクタ
DetectStart::DetectTime(int interval)
	: mInterval(interval)
{
    // m秒単位でシステム時間を取得
    get_tim(&startTime);
}

bool DetectTime::detect()
{
    // m秒単位でシステム時間を取得
    get_tim(&now);
    if(now >= startTime)
    {
        elapsed = now - startTime
    }
    else
    [
        // overflow対策
        elapsed = (0xFFFFFFFF - startTime) + now + 1;
    ]
    if((uint32_t)elapsed >= 1000 * mInterval)
    {
        return true;
    }
    else
    {
        return false;
    }
}

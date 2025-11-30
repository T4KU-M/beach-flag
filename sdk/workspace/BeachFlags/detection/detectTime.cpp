#include "detectTime.h"

// コンストラクタ
DetectTime::DetectTime(int interval)
	: mInterval(interval)
{
    isStartTimeObtained = false;
}

bool DetectTime::detect()
{
    // 最初に呼ばれた時だけstartTimeを記録
    if(!isStartTimeObtained)
    {
        get_tim(&startTime);
        isStartTimeObtained = true;
    }

    // m秒単位でシステム時間を取得
    get_tim(&now);
    if(now >= startTime)
    {
        elapsedTime = now - startTime;
    }
    else
    {
        // overflow対策
        elapsedTime = (0xFFFFFFFF - startTime) + now + 1;
    }
    // 走行体を走らせてみた感じ、1clock = 0.1[ms]くらいの感じがしたので
    // 誰か本環境の1clockを知っている方...
    if((uint32_t)elapsedTime >= 10 * 1000 * mInterval)
    {
        printf("startTime : %ld\n", startTime);
        printf("now : %ld\n", now);
        printf("elapsedTime : %ld\n", elapsedTime);
        printf("%d[s] passed.\n", mInterval);
        return true;
    }
    else
    {
        printf("startTime : %ld\n", startTime);
        printf("now : %ld\n", now);
        printf("elapsedTime : %ld\n", elapsedTime);
        printf("%d[s] has not passed.\n", mInterval);
        return false;
    }
}

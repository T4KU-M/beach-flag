#include "detectCount.h"


// コンストラクタ
DetectCount::DetectCount(int maxCount)
	: mMaxCount(maxCount), mCount(0)
{
}

// 検知結果を確認する
bool DetectCount::detect()
{
    mCount++;
	return (mCount >= mMaxCount);
}

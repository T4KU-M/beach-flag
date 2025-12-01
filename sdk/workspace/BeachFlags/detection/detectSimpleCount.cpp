#include "detectSimpleCount.h"


// コンストラクタ
DetectSimpleCount::DetectSimpleCount(int maxCount)
	: mMaxCount(maxCount), mCount(0)
{
}

// 検知結果を確認する
bool DetectSimpleCount::detect()
{
    mCount++;
	return (mCount >= mMaxCount);
}

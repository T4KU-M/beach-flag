#include "detectCount.h"
#include "module_common.h"

// コンストラクタ
DetectCount::DetectCount()
	:  mTemp(0), mCount(0)
{
}

// 検知結果を確認する
bool DetectCount::detect()
{
	mCount++;
	printf("%d\n",mCount);

	if(mCount < 5){
		mTemp = (int)targetangle;
		return 0;
	}

	if(mCount > 3*100){
		return 1;
	}

	return ((int)targetangle == (int)mTemp)? 0:1;
}

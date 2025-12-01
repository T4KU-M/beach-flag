#include "detectRead.h"
#include "module_common.h"

// コンストラクタ
DetectRead::DetectRead()
	:  mTemp(0), mCount(0)
{
}

// 検知結果を確認する
bool DetectRead::detect()
{
	mCount++;
	printf("%d\n",mCount);

	if(mCount < 5){
		mTemp = (int)targetangle;
		return 0;
	}

	if(mCount > 150){
		return 1;
	}

	return ((int)targetangle == (int)mTemp)? 0:1;
}

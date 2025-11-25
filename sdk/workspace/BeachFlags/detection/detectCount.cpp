#include "detectCount.h"
#include "module_common.h"

// コンストラクタ
DetectCount::DetectCount()
	:  mTemp(0), mCount(0)
{
}

// ビーチフラッグ用魔改造
// 自己位置を更新し終わっているかどうかを検知する
bool DetectCount::detect()
{
	mCount++;
	printf("%d\n",mCount);

	if(mCount < 5){
		mTemp = currentX;
		return false;
	}

	if(mCount > 3*100){
		return true;
	}

	return (currentX == mTemp)? false : true;
}

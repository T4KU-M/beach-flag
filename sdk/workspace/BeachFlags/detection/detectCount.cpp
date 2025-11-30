#include "detectCount.h"
#include "module_common.h"

// コンストラクタ
DetectCount::DetectCount()
	:  mTemp(0), mCount(0), mdetectCount(0)
{
}

// ビーチフラッグ用魔改造
// 自己位置を更新し終わっているかどうかを検知する
bool DetectCount::detect()
{
	mCount++;
	//printf("%d\n",mCount);

	if(mCount < 5){
		return false;
	}

	else if(mCount > 3*100){
		return true;
	}

	else if(mdetectCount == 10)
	{
		return true;
	}

	else
	{
		printf("DetectCount::detect() mCount=%d currentX=%f mTemp=%f\n",mCount,currentX,mTemp);
		if(currentX != mTemp)
		{
			mdetectCount = 0;
			mTemp = currentX;
		}
		else
		{
			mdetectCount++;
		}
		return false;
	}
	
}

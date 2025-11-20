#include "detectgCheck.h"

// コンストラクタ
DetectgCheck::DetectgCheck()
{
    gCheck = false;
}

// gCheckを返す
bool DetectgCheck::detect()
{
	return gCheck;
}

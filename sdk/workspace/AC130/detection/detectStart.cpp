#include "detectStart.h"

// コンストラクタ
DetectStart::DetectStart()
	: mCount(0)
{
}

bool DetectStart::detect()
{
#ifdef MAKE_SIM
	// 検知条件：タッチセンサが押下される
	return gRobot.touchSensor()->isPressed();
#else
	// 検知条件：フォースセンサーが押される
	if (mCount >= 10 && (pup_force_sensor_pressed(gRobot.forceSensor(), 5)!=true))
	{
		printf("スタート検知\n"); // 手を離した瞬間スタート
		return true;
	} 
	if (pup_force_sensor_pressed(gRobot.forceSensor(), 1))
	{
		// 手をかざしている間カウントアップ
		mCount++;
	} 
	else
	{
		// 1秒以内に手を離したらリセット
		mCount = 0;
	} // 1秒以内に手を離したらリセット
	return false;
	// 手を離した瞬間スタート
/*
// 検知条件：超音波センサに1秒間手をかざす
int distance = pup_ultrasonic_sensor_distance(gRobot.ultraSonicSensor());
if (mCount >= 100 && distance == -1)
{
	printf("スタート検知\n"); // 手を離した瞬間スタート
	return true;
} // 手を離した瞬間スタート
if (distance >= 0 && distance < 100)
{
	printf("手をかざしている: %d\n", mCount); // 手をかざしている間カウントアップ
	mCount++;
} // 手をかざしている間カウントアップ
else
{
	printf("手を離した: %d, %d\n", mCount, distance); // 1秒以内に手を離したらリセット
	//pup_ultrasonic_sensor_light_on(gRobot.ultraSonicSensor()); // ライトを消す
	mCount = 0;
} // 1秒以内に手を離したらリセット
return false;
*/
#endif
}

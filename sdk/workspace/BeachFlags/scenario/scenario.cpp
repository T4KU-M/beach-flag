#include "scenario.h"

// コンストラクタ
Scenario::Scenario()
	: sequence(), sequenceSize(), sequenceIndex()
{
}

// デストラクタ
Scenario::~Scenario()
{
	// シナリオに含まれるシーンを全て削除する
	deleteAllScenes();
}

// シナリオに含まれるシーンを全て削除する
void Scenario::deleteAllScenes()
{
	for (int i = 0; i < sequenceSize; i++)
	{
		// 検知のインスタンスを削除
		if (sequence[i].detection != nullptr)
			delete sequence[i].detection;
		
		// 走行のインスタンスを削除
		if (sequence[i].action != nullptr)
			delete sequence[i].action;
	}
}

// シーンを追加する
bool Scenario::append(const Scene &scene)
{
	if (sequenceSize > 99)
		return false; // 最大要素数を超えると失敗(false)

	// シーンを追加
	sequence[sequenceSize++] = scene;
	return true;
}

// シナリオを実行する
bool Scenario::excute()
{
	//シナリオが終了したかどうか判定
	if (sequenceIndex >= sequenceSize)
		return true;

	bool check = false; // シーン切替の判定
	Scene &currentScene = sequence[sequenceIndex]; // 現在実行中のシーン

	// シーンの走行を実行する & モータを駆動する
	currentScene.action->excute();

	// シーンの検知を確認する
	check = currentScene.detection->detect();
	if (check) {
		//printf("Scene No.%d --> %d\n", sequenceIndex, sequenceIndex + 1);
		sequenceIndex++; // 次のシーンに進む
	}

	return false;

	/*例外フローを検知する
	//check |= 例外フロー検知->detect();
	if (check)
		sequenceIndex=sequenceSize; // シナリオを終了させる*/

}

#ifndef SCENARIO_H_
#define SCENARIO_H_

#include "module_common.h"
#include "detect.h"
#include "act.h"

// シーン構造体
struct Scene
{
	Detect *detection;
	Act *action;

	// コンストラクタ（C++の構造体はpublicメンバのクラスと同じ）
	Scene() : detection(nullptr), action(nullptr) {}
	Scene(Detect *detection, Act *action)
		: detection(detection), action(action) {}
};

// シナリオクラス
class Scenario
{
public:
	Scenario();
	~Scenario();
	bool append(const Scene &scene);
	bool excute();

private:
	void deleteAllScenes();

	Scene sequence[100];
	int sequenceSize, sequenceIndex;
};

#endif

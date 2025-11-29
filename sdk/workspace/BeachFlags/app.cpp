#include "app.h"
#include "scenario.h"
#include "robot.h"

#include "detectgCheck.h"
#include "detectSonar.h"
#include "detectMotorAngle.h"
#include "detectBrightness.h"
#include "detectDistance.h"
#include "detectSonarOrDistance.h"

#include "detectStart.h"
#include "detectHsv.h"
#include "detectAngle.h"
#include "detectCount.h"
#include "stay.h"
#include "trace.h"
#include "trace_blue.h"
#include "turn.h"
#include "loopFrontAndBack.h"

// 20250723
#include "act.h"
#include "module_common.h"
#include <unistd.h>

#include "readfile.h"
#include "pipod.h"

#include "detectAngleforpic.h"
#include "pipodforpic.h"

// 追加: ファイル読み込み用 20250904
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void createScenario(Scenario &scenario);
// static void createScenario_slalom(Scenario &scenario);
// static void createScenario_garage(Scenario &scenario);
static void calibration(int &black, int &white);
static void loadPidParams(double &kp, double &ki, double &kd, int num );


/////////キャリブレーションで書き換え///////////////////////////////////////////////////////////////////////
//#define CARIBRATION // コメント外すと実行
#define SETPIDFROMFILE // コメント外すと実行

// 白黒
#define WHITE_R 1000 // G5
#define BLACK_R 40	 // G5

// 青色カラー検知 +-40とる設定　ダブルループで使う
#define BLUE_H 200
#define BLUE_S 200
#define BLUE_V 403

// 赤色カラー検知
#define RED_H 350

// PIDの初期値ファイルで書き換えできる(pid数字.txt)
//1:直線 2:カーブ 3:でかい丸 4:小さい丸
#define KP1 0.35
#define KI1 0.02
#define KD1 5.0
#define KP2 0.0
#define KI2 0.2
#define KD2 0.025
#define KP3 1.2
#define KI3 0.0
#define KD3 0.15
#define KP4 1.8
#define KI4 0.2
#define KD4 0.2
////////キャリブレーションで書き換え///////////////////////////////////////////////////////////////////////

// 自動速度走行のプラスナイマス（一応残しとくけどわからん：檜山）
#define SPEED_DELTA_MINUS 10
#define SPEED_DELTA_PLUS 3

// 追いかけるボトルの色を選択
#define TARGET_BOTTLE_COLOR BLUE 
// #define TARGET_BOTTLE_COLOR RED 

// メインタスク
extern void mainTask(intptr_t exinf)
{
	printf("めいんたすく\n");
	act_tsk(SCENARIO_TASK);
	sta_cyc(SCENARIO_CYCL);
	slp_tsk();
	//...
	ext_tsk();
}

// シナリオタスク
void scenarioTask(intptr_t unused)
{
	printf("シナリオタスク\n");

	/*モーターの初期化*/
	pup_motor_setup(gRobot.leftMotor(), PUP_DIRECTION_COUNTERCLOCKWISE, true);
	pup_motor_setup(gRobot.rightMotor(), PUP_DIRECTION_CLOCKWISE, true);

	/*キャリブレーションの値かdefine値を使うかの選択*/
	gBlack = BLACK_R;
	gWhite = WHITE_R;
	BLUE.maxH = BLUE_H + 50;
	BLUE.minH = BLUE_H - 50;
	BLUE.maxS = BLACK_R + 200;
	BLUE.minS = BLACK_R + 30;
	BLUE.maxV = 1023;
	BLUE.minV = 400;
	RED.maxH = RED_H + 50;
	RED.minH = RED_H - 50;
	RED.maxS = BLACK_R + 200;
	RED.minS = BLACK_R + 30;
	RED.maxV = 1023;
	RED.minV = 400;
	/*
	HSV
	青　211 168 59
	赤　350 160 70
	緑　150 120 52
	黒　200 150 23
	白　200 30 101
	*/
#ifdef CARIBRATION
	calibration(gBlack, gWhite);
#endif /*CARIBRATION*/

	// シナリオを作成する
	bool complete = false;
	Scenario *pScenario = new Scenario();
	createScenario(*pScenario);
	do
	{
		// スタートからゴールまでのシナリオを実行する
		complete = pScenario->excute();
		slp_tsk();
	} while (!complete);
	delete pScenario;

	pup_motor_set_power(gRobot.leftMotor(), 0);
	pup_motor_set_power(gRobot.rightMotor(), 0);

	wup_tsk(MAIN_TASK);
	ext_tsk();
}

// シナリオを作成する.
static void createScenario(Scenario &scenario)
{
	// const int darkThreshold = gBlack + 20;
	// const int grayThreshold = (gWhite + gBlack) / 2 - 10;
	double Kp, Ki, Kd;
	int speedMin, speedMax;
	int steeringMin, steeringMax;
	// int brihgtnessMin, brightnessMax;
	int fixedTurningAmount;
	int minH, maxH, minS, maxS, minV, maxV;
	double threTravelDistance;
	LeftOrRight lineEdge, direction;
	Target target;

	// PID の初期値（デフォルト値）20250904////////////////////////////////////////////////////////////////////////////////////
	double kp_test = KP1;
	double ki_test = KI1;
	double kd_test = KD1;
	double kp2_test = KP2;
	double ki2_test = KI2;
	double kd2_test = KD2;
	double kp3_test = KP3;
	double ki3_test = KI3;
	double kd3_test = KD3;
	double kp4_test = KP4;
	double ki4_test = KI4;
	double kd4_test = KD4;
	int fix_speed = 15; // 直線のときに足す速度
	int fix_speed_curb = 45;		  // カーブのときに足す速度
	int fix_speed_loop = 55;	  // ダブルループのときに足す速度
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef SETPIDFROMFILE
	// 追加: pid.txt から Kp, Ki, Kd を読み込む（存在しなければ上のデフォルトを使用）
	loadPidParams(kp_test, ki_test, kd_test, 1);
	loadPidParams(kp2_test, ki2_test, kd2_test, 2);
	loadPidParams(kp3_test, ki3_test, kd3_test, 3);
	loadPidParams(kp4_test, ki4_test, kd4_test, 4);
#endif /*SETPIDFROMFILE*/
	///////////////////////ここが本番のシナリオの中身////////////////////////////////////////////////////////
	/*
	動作：待機
	終了：ボタン押される
	*/
	scenario.append({new DetectStart(),
					 new Stay()});
	/* 動作：ぴぽっど 終了：角度 */
	scenario.append({new DetectAngle(178),
					 new Pipod(Left)});
	/* 動作：ファイル読み込み 終了：時間*/
	scenario.append({new DetectCount(),
					 new Readfile()});
	/* 動作：ピポッド 終了：角度 */
	scenario.append({new DetectAngleforpic(target = Marker),
					 new Pipodforpic(target = Marker)});				 
	/* 動作：直進 終了：距離*/
	scenario.append({new DetectDistance(threTravelDistance = 2000),
					 new Turn(fixedTurningAmount = 0, speedMin = 100, speedMax = 100)});

	/* 動作：ファイル読み込み 終了：時間*/
	scenario.append({new DetectCount(),
					 new Readfile()});
	/* 動作：直進 終了：距離*/
	scenario.append({new DetectDistance(threTravelDistance = 2100),
					 new Turn(fixedTurningAmount = 0, speedMin = 100, speedMax = 100)});
	/* 動作：ピポッド 終了：角度 */
	scenario.append({new DetectAngleforpic(target = Marker),
					 new Pipodforpic(target = Marker)});
	//停止
	scenario.append({new DetectStart(),
					 new Stay()});

	/*
	// 走行：右ライントレース
	// 検知：青色を検知
	scenario.append({new DetectHsv(minH = gBlueMinH, maxH = gBlueMaxH, minS = gBlueMinS, maxS = gBlueMaxS, minV = gBlueMinV, maxV = gBlueMaxV),
					 new Trace(lineEdge = Right, Kp = kp_test, Ki = ki_test, Kd = kd_test, steeringMin = -90, steeringMax = 90, speedMin = 55 + fix_speed_curb - SPEED_DELTA_MINUS, speedMax = 55 + fix_speed_curb + SPEED_DELTA_PLUS)});

	*/
}

///////////////////////キャリブレーション↓↓//////////////////////////////////////////////////////////////
static void calibration(int &black, int &white)
{
	// ev3api::Motor arm(PORT_A);
	pup_motor_t *arm = pup_motor_get_device(PBIO_PORT_ID_C);
	DetectStart detection1;
	DetectStart detection2;
	DetectStart detection3;
	DetectStart detection4;
	int count;

	// 明るさを取得する関数（ラムダ式）
	auto getbr = [&]()
	{
		// rgb_raw_t rgb;
		// pup_color_rgb_t rgb;

		// gRobot.colorSensor()->getRawColor(rgb);
		pup_color_rgb_t rgb = pup_color_sensor_rgb(gRobot.colorSensor());

		return (int)rgb.r;
	};
	// アームを上げて下げる関数（ラムダ式）
	auto movearm = [&]()
	{
		pup_motor_set_power(arm, +20);
		for (int t = 0; t < 100; t++)
		{
			slp_tsk();
		}

		pup_motor_set_power(arm, -30);
		for (int t = 0; t < 100; t++)
		{
			slp_tsk();
		}

		pup_motor_set_power(arm, 0);
	};

	// 表示
	printf("ボタンを押してください\n");
	// 検知するまで何もしない
	while (!detection1.detect())
	{
		slp_tsk();
	}
	// 表示
	printf("キャリブレーション 開始!\n");
	// アームを上げて下げる
	movearm();
	// 白を取得する
	printf("白を取得\n");
	count = 0;
	while (!detection2.detect())
	{
		white = getbr();
		// 0.5秒毎に明るさを表示
		if (++count == 50)
		{
			count = 0;
			printf("\r明るさ %3d", white);
			fflush(stdout);
		}
		slp_tsk();
	}
	// アームを上げて下げる
	movearm();
	// 黒を取得する
	printf("\n黒を取得\n");
	count = 0;
	while (!detection3.detect())
	{
		black = getbr();
		// 0.5秒毎に明るさを表示
		if (++count == 50)
		{
			count = 0;
			printf("\r明るさ %3d", black);
			fflush(stdout);
		}
		slp_tsk();
	}
	printf("\n黒 = %d, 白 = %d\n", black, white);
	// アームを上げて下げる
	movearm();

	// 青を取得する
	printf("\n青を取得\n");
	int h, s, v;
	HsvMeasure mHsvMeasure;
	while (!detection4.detect())
	{
		mHsvMeasure.getHSV(h, s, v);
		// 0.5秒毎にhsvを表示
		if (++count == 50)
		{
			count = 0;
			printf("\rhsv %3d %3d %3d", h, s, v);
			fflush(stdout);
		}
		slp_tsk();
	}
	BLUE.maxH = h + 50;
	BLUE.minH = h - 50;
	BLUE.maxS = black + 200;
	BLUE.minS = black + 30;
	BLUE.maxV = 1024;
	BLUE.minV = 400;
	// アームを上げて下げる
	movearm();

	// 赤を取得する
	printf("\n赤を取得\n");
	int h, s, v;
	HsvMeasure mHsvMeasure;
	while (!detection4.detect())
	{
		mHsvMeasure.getHSV(h, s, v);
		// 0.5秒毎にhsvを表示
		if (++count == 50)
		{
			count = 0;
			printf("\rhsv %3d %3d %3d", h, s, v);
			fflush(stdout);
		}
		slp_tsk();
	}
	RED.maxH = h + 50;
	RED.minH = h - 50;
	RED.maxS = black + 200;
	RED.minS = black + 30;
	RED.maxV = 1024;
	RED.minV = 400;
	// アームを上げて下げる
	movearm();

	// 表示
	printf("キャリブレーション 完了!\n");
}
//////////////////////////////キャリブレーション↑↑/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////txtからPIDを読み込む↓↓/////////////////////////////////////////////////////////////////////////////////////////////
// 追加: PIDファイルから読み込むヘルパー 20250904
// 対応フォーマット例：
//   1) 1.2 0.05 0.15
//   2) Kp=1.2 Ki=0.05 Kd=0.15
//   3) Kp:1.2 / Ki:0.05 / Kd:0.15
// 空白区切り、カンマ区切り、改行区切りにも対応（順序 Kp, Ki, Kd）
// 見つからない場合はデフォルト値を使用
static bool loadPidFromPath(const char *path, double &kp, double &ki, double &kd)
{
	FILE *fp = fopen(path, "r");
	if (!fp)
		return false;

	char line[256];
	double tmpKp = kp, tmpKi = ki, tmpKd = kd;
	bool hasKp = false, hasKi = false, hasKd = false;

	// 数値を順に拾うためのバッファ
	double nums[3];
	int nNumbers = 0;

	while (fgets(line, sizeof(line), fp))
	{
		// キー指定での取得を試みる
		double v;
		if (!hasKp && sscanf(line, "Kp%*[^0-9.-]%lf", &v) == 1)
		{
			tmpKp = v;
			hasKp = true;
			continue;
		}
		if (!hasKi && sscanf(line, "Ki%*[^0-9.-]%lf", &v) == 1)
		{
			tmpKi = v;
			hasKi = true;
			continue;
		}
		if (!hasKd && sscanf(line, "Kd%*[^0-9.-]%lf", &v) == 1)
		{
			tmpKd = v;
			hasKd = true;
			continue;
		}

		// 行内のあらゆる数値を順に抽出（Kp/Ki/Kdの順に採用）
		char *p = line;
		while (*p && nNumbers < 3)
		{
			char *endptr = nullptr;
			double num = strtod(p, &endptr);
			if (endptr != p)
			{
				nums[nNumbers++] = num;
				p = endptr;
			}
			else
			{
				++p;
			}
		}
	}

	fclose(fp);

	// キーが取れなかった場合は数値の順序で補完
	if (!hasKp && nNumbers >= 1)
	{
		tmpKp = nums[0];
		hasKp = true;
	}
	if (!hasKi && nNumbers >= 2)
	{
		tmpKi = nums[1];
		hasKi = true;
	}
	if (!hasKd && nNumbers >= 3)
	{
		tmpKd = nums[2];
		hasKd = true;
	}

	if (hasKp && hasKi && hasKd)
	{
		kp = tmpKp;
		ki = tmpKi;
		kd = tmpKd;
		return true;
	}
	return false;
}

static void loadPidParams(double &kp, double &ki, double &kd, int num )
{
	// まずは実行ディレクトリの pid.txt を探す
		const char* candidates[1];  // まず外で宣言

	if (num == 1) {
		candidates[0] = "pid1.txt";
	} else if (num == 2) {
		candidates[0] = "pid2.txt";
	} else if (num == 3) {
		candidates[0] = "pid3.txt";
	} else if(num == 4){
		candidates[0] = "pid4.txt";
	}

	
	bool loaded = false;
	for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i)
	{
		if (loadPidFromPath(candidates[i], kp, ki, kd))
		{
			printf("PID loaded from %s: Kp=%.6f Ki=%.6f Kd=%.6f\n", candidates[i], kp, ki, kd);
			loaded = true;
			break;
		}
	}
	if (!loaded)
	{
		printf("PID file not found or invalid. Using defaults: Kp=%.6f Ki=%.6f Kd=%.6f\n", kp, ki, kd);
	}
}
//////////////////////////////txtからPIDを読み込む↑↑/////////////////////////////////////////////////////////////////////////////////////////////

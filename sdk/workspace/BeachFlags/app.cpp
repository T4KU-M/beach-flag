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

// ビーチフラッグ用
#include "detectTime.h"
#include "localizer.h"
#include "turnByLocalizer.h"
#include "detectAngleForCurrentTargetValue.h"
#include <iostream>
#include <fstream>

// createScenario()に渡すパラメータ群を保持する構造体
struct import_params
{
	int targetColor;				// 何色のゴールを目指すか(0:R/1:B)
	int deviceForAdjust;			// フィードバック走行にカメラを使うか、ジャイロを使うか(0:カメラ/1:ジャイロ)
	int speed;						// 走行スピード(1~100)
	int intervalForGettingFile;		// 何秒に一度ジャイロorカメラからファイルを取得するか(1~10[s])
	int amountOfAdjust;				// フィードバック制御時の制御量(1~10)
};

static void createScenario(Scenario &scenario, import_params &importParams, Localizer &localizer);
// static void createScenario(Scenario &scenario)
// static void createScenario_slalom(Scenario &scenario);
// static void createScenario_garage(Scenario &scenario);
static void calibration(int &black, int &white);
static void loadPidParams(double &kp, double &ki, double &kd, int num );

// ビーチフラッグ用に新規定義
static void waitForForceSensor();
static bool isForceSensorPressed(DetectStart &detectForceSensor);
static void getParamsFromFile(import_params &importParams);
static std::string importFilePath = "/home/AC130/RasPike-ART/sdk/workspace/param_beach-flag/param_beach-flag.txt";

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


// 赤色カラー検知 ビーチフラッグ用
#define RED_H 350
#define RED_S 0
#define RED_V 0

// 緑色カラー検知 ビーチフラッグ用
#define GREEN_H 0
#define GREEN_S 0
#define GREEN_V 0

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
	
	gBlueMaxH = BLUE_H + 100;
	gBlueMinH = BLUE_H - 100;
	gBlueMaxS = BLACK_R + 200;
	gBlueMinS = BLACK_R + 30;
	gBlueMaxV = 1023;
	gBlueMinV = 400;
#ifdef CARIBRATION
	calibration(gBlack, gWhite);
#endif /*CARIBRATION*/

	// 以下、ビーチフラッグ用に無限ループに変更
	while(true){
		// 新規関数 フォースセンサが押下されるまで待機
		waitForForceSensor();
		printf("BF: start!\n");
		
		// param_beach-flag.txt よりパラメータ値を取得
		getParamsFromFile(importParams);

		// 目的地の色を設定
		switch(importParams.targetColor)
		{
			// RED
			case 0:
				MaxH = gRedMaxH;
				MinH = gRedMinH;
				MaxS = gRedMaxS;
				MinS = gRedMinS;
				MaxV = gRedMaxV;
				MinV = gRedMinV;
			break;

			// BLUE
			case 1:
				MaxH = gBlueMaxH;
				MinH = gBlueMinH;
				MaxS = gBlueMaxS;
				MinS = gBlueMinS;
				MaxV = gBlueMaxV;
				MinV = gBlueMinV;
			break;

			default:
			break;
		}

		// 目的のカラーゾーンに到着したか検知するクラス
		DetectHsv *detectColor = new DetectHsv(MinH, MaxH, MinS, MaxS, MinV, MaxV);

		// フォースセンサ検知→中断用
		// ここでインスタンス化しとかないとisForceSensorPressedが呼び出されるたびに状態がリセットされてしまう
		DetectStart *detectForceSensor = new DetectStart();

		// シナリオ作成用変数
		Scenario *pScenario = new Scenario();
		Localizer *pLocalizer = new Localizer();
		currentX = -1;
		currentY = -1;
		currentTheta = -1;

		// シナリオを作成する
		createScenario(*pScenario, importParams, *pLocalizer);

		// 終了フラグ
		bool complete = false;

		do
		{
			// 新規関数 フォースセンサが押されていないかチェック
			if(isForceSensorPressed(*detectForceSensor))
			{
				printf("BF: stop!\n");
				break;
			}

			// 目的のカラーゾーンに到着しているかチェック
			if(detectColor -> detect())
			{
				printf("BF: arrived!\n");
				break;
			}

			// スタートからゴールまでのシナリオを実行する
			complete = pScenario->excute();
			slp_tsk();
		} while (!complete);

		delete pScenario;
		delete pLocalizer;
		delete detectColor;
		delete detectForceSensor;

		pup_motor_set_power(gRobot.leftMotor(), 0);
		pup_motor_set_power(gRobot.rightMotor(), 0);
	}

	// Mainタスク終了
	wup_tsk(MAIN_TASK);
	ext_tsk();
}

// シナリオを作成する.
static void createScenario(Scenario &scenario, import_params &importParams, Localizer &localizer)
{
	// const int darkThreshold = gBlack + 20;
	// const int grayThreshold = (gWhite + gBlack) / 2 - 10;
	double Kp, Ki, Kd;
	int speedMin, speedMax;
	int steeringMin, steeringMax;
	// int brihgtnessMin, brightnessMax;
	int fixedTurningAmount;
	int TurningAmountForBeachFlag = 80; // ビーチフラッグ用に定数を設定 半時計回りが旋回量正
	int minH, maxH, minS, maxS, minV, maxV;
	double threTravelDistance;
	LeftOrRight lineEdge, direction;
	Target target;

	// ビーチフラッグ用に変数追加
	int targetColor  			= importParams.targetColor;					// 何色のゴールを目指すか(0:R/1:B)
	int deviceForAdjust 		= importParams.deviceForAdjust;				// フィードバック走行にカメラを使うか、ジャイロを使うか(0:カメラ/1:ジャイロ)
	int speed	 				= importParams.speed;						// 走行スピード(1~100)
	int intervalForGettingFile 	= importParams.intervalForGettingFile;		// 何秒に一度ジャイロorカメラからファイルを取得するか(1~10[s])
	double amountOfAdjust 		= importParams.amountOfAdjust / 5; 			// フィードバック制御時の制御量(1~10) 0.2 <= kp <= 2.0 の間くらいで動かすとする

	// log
	printf("BF: createScenario() called with params - targetColor: %d, deviceForAdjust: %d, speed: %d, intervalForGettingFile: %d, amountOfAdjust: %f\n",
		   targetColor, deviceForAdjust, speed, intervalForGettingFile, amountOfAdjust);

	// speedから秒速[mm]およびintervalForGettingFile[s]間走行する際の移動距離を算出
	// 実験によりspeed = PWM = 50の時におよそ210mm/sで走行することが分かっている
	// 単位速度はPWMに比例すると仮定して、4.2mm/(s*PWM) である
	// double speed_mm_per_s = 4.2 * speed; // [mm/s]
	double speed_mm_per_s = 3.8 * speed; // [mm/s]
	double travelDistance = speed_mm_per_s * intervalForGettingFile; // [mm]


	// ビーチフラッグ 目標地点の座標
	double goalX = 4000.0; // スタート時、背を向けているところが基準になるので進行方向はマイナスの値
	double goalY = 0.0; 

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

	/* 動作：ぴぽっど 終了：角度 */
	scenario.append({new DetectAngle(170),
	  				 new Pipod(Left)});
	
	/* 慣性で動くのを止めたい 動作：停止 終了：指定時間経過*/
	scenario.append({new DetectTime(intervalForGettingFile),
	 	 			new Stay()});
	
	if(deviceForAdjust == 1) // ジャイロ使用の場合
	{
		/* 動作：自己位置を(0, 0)にリセット 終了：自己位置更新完了 */
		scenario.append({new DetectCount(),
		  			 	 new TurnByLocalizer(0, 0, 0, localizer)});
	}

	// 走行 → 停止&情報取得 を何回繰り返すかわからない
	// 400cm直進なので、まあ20セットくらいシナリオに入れておけば十分か？
	for(int i = 0; i < 20; i++)
	{
		/* カメラ or ジャイロから情報取得 */
		if(deviceForAdjust == 0) // カメラの場合
		{
			/* 動作：カメラから情報取得 終了：?? */
			// TODO: カメラの利用方法確認

			// scenario.append({new DetectCount(),
			// 		 new Stay()});	
			// scenario.append({new DetectRead(),
			// 					new Readfile()});
			// scenario.append({new DetectAngleforpic(target = Marker),
			// 					new Pipodforpic(target = Marker)});
		}
		else // ジャイロの場合
		{
			/* 動作：ジャイロで自己位置推定 → 目標ターン角度を計算してcurrentTargetThetaに格納 → 一定のPWMで旋回 
			   終了：currentTargetThetaに格納されている角度分のターンが完了 */
			scenario.append({new DetectAngleForCurrentTargetValue(),
			  		 	 	new TurnByLocalizer(goalX, goalY, TurningAmountForBeachFlag, localizer)});
		}

		/* 動作：直進 終了：指定時間走行*/
		scenario.append({new DetectDistance(travelDistance),
		 			 	new Turn(fixedTurningAmount = 0, speedMin = speed, speedMax = speed, Kp = amountOfAdjust)});

		/* 実験用 動作：停止 終了：指定時間経過*/
		scenario.append({new DetectTime(intervalForGettingFile),
		  			 	new Stay()});
	}			 
	
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


// ビーチフラッグ用 新規定義
void waitForForceSensor()
{
	DetectStart detectStart;
	while(true)
	{
		if(detectStart.detect())
		{
			break;
		}
		slp_tsk();
	}
}

// ビーチフラッグ用 新規定義
bool isForceSensorPressed(DetectStart &detectForceSensor)
{
	if(detectForceSensor.detect())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// ビーチフラッグ用 新規定義
void getParamsFromFile(import_params &importParams)
{
	/*
	int targetColor;				// 何色のゴールを目指すか(0:R/1:B)
	int deviceForAdjust;			// フィードバック走行にカメラを使うか、ジャイロを使うか(0:カメラ/1:ジャイロ)
	int speed;						// 走行スピード(1~100)
	int intervalForGettingFile;		// 何秒に一度ジャイロorカメラからファイルを取得するか(1~10[s])
	int amountOfAdjust;				// フィードバック制御時の制御量(1~10)
	*/
	
	// デフォルト値
	// TODO: デフォルト値決定
	import_params defaultValues = {1, 0, 0, 1, 1};

	std::ifstream paramFile(importFilePath);
	std::vector<int> inputs;
	if(!paramFile.is_open())
	{
		printf("BF: failed to get parameters from file.\n");

		// ファイルが無ければデフォルト値を設定
		importParams.targetColor 			= defaultValues.targetColor;
		importParams.deviceForAdjust 		= defaultValues.deviceForAdjust;
		importParams.speed 					= defaultValues.speed;
		importParams.intervalForGettingFile = defaultValues.intervalForGettingFile;
		importParams.amountOfAdjust 		= defaultValues.amountOfAdjust;
	}
	else
	{
		printf("BF: succeed to get parameters from file.\n");

		std::string line;
		while(std::getline(paramFile, line))
		{
			try
			{
				int param = std::stoi(line);
				inputs.push_back(param);
				printf("BF: param = %d\n", param);
			}
			catch(const std::exception& e)
			{
				// 整数型に変換できなかった場合-1を入れておく
				printf(e.what());
				inputs.push_back(-1);
			}
		}

		// 最大値・最小値と比較し、仕様範囲外の場合デフォルト値を設定
		importParams.targetColor 			= (0 <= inputs[0] && inputs[0] <= 2)? 	inputs[0] : defaultValues.targetColor;
		importParams.deviceForAdjust 		= (0 <= inputs[1] && inputs[1] <= 1)? 	inputs[1] : defaultValues.deviceForAdjust;
		importParams.speed 					= (1 <= inputs[2] && inputs[2] <= 100)? inputs[2] : defaultValues.speed;
		importParams.intervalForGettingFile = (1 <= inputs[3] && inputs[3] <= 10)? 	inputs[3] : defaultValues.intervalForGettingFile;
		importParams.amountOfAdjust 		= (1 <= inputs[4] && inputs[4] <= 10)? 	inputs[4] : defaultValues.amountOfAdjust;
	}
}


//////////////////////////////txtからPIDを読み込む↑↑/////////////////////////////////////////////////////////////////////////////////////////////

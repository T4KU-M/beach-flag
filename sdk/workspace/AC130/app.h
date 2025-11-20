#ifdef __cplusplus
extern "C" {
#endif

//#include "ev3api.h"
#include "spikeapi.h"

// task stack size
#ifndef STACK_SIZE
#define STACK_SIZE	    1000000
#endif

// task priorities
#define MAIN_PRIORITY		TMIN_APP_TPRI + 2
#define SCENARIO_PRIORITY	TMIN_APP_TPRI + 1

// task function prototypes
#ifndef TOPPERS_MACRO_ONLY
extern void mainTask(intptr_t exinf);
extern void scenarioTask(intptr_t exinf);
#endif

#ifdef __cplusplus
}
#endif
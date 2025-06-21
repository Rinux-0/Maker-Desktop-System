#include "tmp.h"

#include "ddef.h"
#include "ttool.h"

#define TMP_NAME ws2812
#include MY_HEADER_PATH(ExBoard, TMP_NAME, h)



void tmp_init(void) {
	tool_init();
	INIT(TMP_NAME)();
}


void tmp_oneloop(void) {
	tool_watchdog_kick();
	ONELOOP(TMP_NAME)();
}


void tmp_exit(void) {
	tool_exit();
	EXIT(TMP_NAME)();
}

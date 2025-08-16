#include "tmptest.h"

#include "ddef.h"
#include "ttool.h"

#include "tmptest_def.h"
#include STR(TEST_TMPTEST_NAME.h)



void tmptest_init(void) {
	INIT(TEST_TMPTEST_NAME)();
}


void tmptest_oneloop(void) {
	ONELOOP(TEST_TMPTEST_NAME)();
}


void tmptest_exit(void) {
	tool_exit();
	EXIT(TEST_TMPTEST_NAME)();
}

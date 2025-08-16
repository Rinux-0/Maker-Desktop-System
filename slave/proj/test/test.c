#include "test.h"
#include "test_def.h"

#include "ddef.h"
#include "ttool.h"

#include MY_HEADER_PATH(TEST_NAME, TEST_NAME, h)



void test_init(void) {
	INIT(TEST_NAME)();

	LOG("");
}


void test_oneloop(void) {
	ONELOOP(TEST_NAME)();
}


void test_exit(void) {
	EXIT(TEST_NAME)();
}

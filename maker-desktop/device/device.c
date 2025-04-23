#include "device.h"
#include "device_def.h"

#include "ddef.h"
#include "ttool.h"

#include MY_HEADER(DEVICE_NAME, DEVICE_NAME, h)



void device_init(void) {
	INIT(DEVICE_NAME)();
}


void device_oneloop(void) {
	ONELOOP(DEVICE_NAME)();
}


void device_exit(void) {
	EXIT(DEVICE_NAME)();
}

#include "receiver.h"

#include "ddef.h"
#include "ttool.h"

#include "core/rcv_core.h"



void receiver_init(void) {
	rcv_init_pin();
	rcv_init_int_cb();
}


void receiver_oneloop(void) {}


void receiver_exit(void) {}

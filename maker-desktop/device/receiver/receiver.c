#include "receiver.h"

#include "ddef.h"
#include "ttool.h"

#include "core/rcv_core.h"



void receiver_init(void) {
	rcv_init_pin();

	sle_set_r_cb(rcv_sle_r_int_handler);
	// uart_set_r_cb( ... );
}


void receiver_oneloop(void) {}


void receiver_exit(void) {}

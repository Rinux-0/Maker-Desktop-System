#include "receiver.h"

// #include "ddef.h"
// #include "ttool.h"

#include "comm.h"
#include "ssle.h"



void receiver_init(void) {
	sle_set_r_cb(sle_r_int_uart_handler);
}


void receiver_oneloop(void) {}


void receiver_exit(void) {}

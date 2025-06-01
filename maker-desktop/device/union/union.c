#include "union.h"

#include "ddef.h"
#include "ttool.h"

#include "receiver/receiver.h"



void union_init(void) {
	receiver_init();
}


void union_oneloop(void) {
	receiver_oneloop();
}


void union_exit(void) {
	receiver_exit();
}

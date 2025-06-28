#include "wwifi.h"

#include "ddef.h"
#include "ttool.h"

#include "udp_client.h"



void wifi__init(void) {
	udp_client_init();
}


void wifi_oneloop(void) {
	udp_client_oneloop();
}


void wifi_exit(void) {
	udp_client_exit();
}


void wifi_write(const u8* data, u16 len) {
	udp_client_send_data((char*)data, len);
}

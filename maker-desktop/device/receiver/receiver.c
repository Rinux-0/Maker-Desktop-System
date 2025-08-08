#include "receiver.h"

#include "ddef.h"
#include "ttool.h"

#include "core/rcv_core.h"

#include "ssle.h"
#include "uuart.h"
#include "wwifi.h"



u8 ip_is_valid(c8* ip) {
	c8* sharp[2] = {};
	c8* dot[3] = {};

	// 首尾
	if (ip){
		sharp[0] = strnstr(ip, "#", 24);
		DATA("\n\t??????  1  ??????\n\n");
	}if (sharp[0]) {
		sharp[1] = strnstr(sharp[0] + 1 + 7, "#", 12);
		DATA("\n\t??????  2  ??????\n\n");
	}

	// 三点
	if (sharp[1]){
		dot[0] = strnstr(sharp[0] + 1 + 1, ".", 4);
		DATA("\n\t??????  3  ??????\n\n");
	}if (dot[0]) {
		dot[1] = strnstr(dot[0] + 1 + 1, ".", 4);
		DATA("\n\t??????  4  ??????\n\n");
	}	if (dot[1]) {
		dot[2] = strnstr(dot[1] + 1 + 1, ".", 4);
		DATA("\n\t??????  5  ??????\n\n");
	}

	// 缺少 值判断
	if (dot[2]) {
		return sharp[1] - sharp[0] - 1;
	} else {
		return 0;
	}
}

//&               &#192.168.220.49#
void uart_r_int_set_wifi_ip_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	c8* buff = (c8*)buffer;

	u8 ip_length = ip_is_valid(buff);
	if (ip_length == 0) {
		ERROR("\n\tip error\n\n");
		return;
	}

	c8 ip[16] = {};
	strncpy(ip, buff+1, ip_length);

	wifi_modify_cfg(NULL, NULL, (u8*)ip, -1);
}


void receiver_init(void) {
	rcv_init_pin();

	uart_init(UART_BUS_ID(0), false);

	sle_set_r_cb(rcv_sle_r_int_handler);
	uart_set_r_cb(UART_BUS_ID(0), uart_r_int_set_wifi_ip_handler);
}


void receiver_oneloop(void) {}


void receiver_exit(void) {}

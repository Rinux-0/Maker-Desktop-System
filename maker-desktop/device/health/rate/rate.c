#include "rate.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"



u8 data_breath;
u8 data_heart;



static void rate_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;

	if (buff[0] != 0x53 || buff[1] != 0x59 ||		// frame_head
		buff[8] != 0x54 || buff[9] != 0x43) {		// frame_tail
		// LOG("\n\trate: uart_cb error\n\n");
		return;
	}
	/// @todo WIFI 发给上位机
	if (buff[2] == 0x81) {
		data_breath = buff[2];
		LOG("\n\tbreath: %-d.%d\n\n", data_breath);
	} else if (buff[2] == 0x85) {
		data_heart = buff[2];
		LOG("\n\theart: %-d.%d\n\n", data_heart);
	}
}


static void rate_write_get_req(void) {
	const u8 data[] = "\xA1\xAB";

	uart_write(1, data, sizeof(data) - 1);

	// LOG("");
}


void rate_init(void) {
	uart_set_r_cb(1, rate_uart_r_int_handler);
}


void rate_oneloop(void) {
	rate_write_get_req();
}


void rate_exit(void) {}

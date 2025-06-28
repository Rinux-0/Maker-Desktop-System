#include "rate.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"



static u8 str_breath[4];
static u8 str_heart[5];
static bool is_wating;



static void rate_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;

	if (buff[0] != 0x53 || buff[1] != 0x59 ||		// frame_head
		buff[8] != 0x54 || buff[9] != 0x43) {		// frame_tail
		LOG("\n\trate: uart_cb error\n\n");
		return;
	}

	/// @todo WIFI 发给上位机
	if (buff[2] == 0x81) {
		sprintf((c8*)str_breath, "b%02d", buff[2]);
		sle_write(receiver, str_breath, sizeof(str_breath) - 1);

		LOG("\n\tbreath: %+d\n\n", str_breath);
	} else if (buff[2] == 0x85) {
		sprintf((c8*)str_heart, "h%03d", buff[2]);
		sle_write(receiver, str_heart, sizeof(str_heart) - 1);

		LOG("\n\theart: %+d\n\n", str_heart);
	}

	is_wating = false;
}


static void rate_write_get_req(void) {
	const u8 data[] = "\xA1\xAB";

	uart_write(UART_BUS_ID(1), data, sizeof(data) - 1);
	is_wating = true;

	while (is_wating)
		tool_delay_m(10);

	// LOG("");
}


void rate_init(void) {
	uart_set_r_cb(UART_BUS_ID(1), rate_uart_r_int_handler);
}


void rate_oneloop(void) {
	rate_write_get_req();
}


void rate_exit(void) {}

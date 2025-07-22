#include "rate.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



static u8 str_breath[4];
static u8 str_heart[5];



static void rate_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;

	if (buff[0] != 0x53 || buff[1] != 0x59 ||		// frame_head
		buff[8] != 0x54 || buff[9] != 0x43) {		// frame_tail
		// ERROR("\n\trate: uart_cb error\n\n");
		return;
	}

	/// @todo WIFI 发给上位机
	if (buff[2] == 0x81) {
		sprintf((c8*)str_breath, "b%02d", buff[6]);
		sle_write(pc, str_breath, sizeof(str_breath) - 1);

		LOG("\n\tbreath: %+d\n\n", atoi((c8*)str_breath + 1));
	} else if (buff[2] == 0x85) {
		sprintf((c8*)str_heart, "h%03d", buff[6]);
		sle_write(pc, str_heart, sizeof(str_heart) - 1);

		LOG("\n\theart: %+d\n\n", atoi((c8*)str_heart + 1));
	}
}


static void rate_init(void) {
	uart_set_r_cb(UART_BUS_ID(1), rate_uart_r_int_handler);
}


static void rate_oneloop(void) {
	tool_sleep_m(1);
}


static void rate_exit(void) {}



static void* rate(const c8* arg) {
	unused(arg);

	rate_init();
	while (1)
		rate_oneloop();
	rate_exit();

	return NULL;
}


void rate_entry(void) {
	osThreadAttr_t attr = {
		.name = "rate",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)rate, NULL, &attr)) {
		ERROR("Failed to create rate sub_thread");
	}
}

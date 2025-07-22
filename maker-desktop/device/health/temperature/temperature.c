#include "temperature.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



static u8 str_temperature[7];
static volatile bool is_wating;


static void temperature_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	const c8* buff = (const c8*)buffer;
	s16 data = atoi(buff);

	sprintf((c8*)str_temperature, "t%+05.1f", data / 10.0f);
	sle_write(pc, str_temperature, sizeof(str_temperature) - 1);

	is_wating = false;

	/// @todo WIFI 发给上位机
	DATA("\n\ttemp: %+d.%d C\n\n", data % 1000 / 10, abs(data) % 10);
}


static void temperature_write_get_req(void) {
	const u8 data[] = "\xA1\xAB";

	uart_write(UART_BUS_ID(2), data, sizeof(data) - 1);
	is_wating = true;

	bool strt = g_time_wait_0s1;
	while (is_wating) {
		tool_sleep_m(1);
		if (strt != g_time_wait_0s1) {
			// DATA("\n\ttemp: error_timeout\n\n");
			break;
		}
	}

	// LOG("");
}


static void temperature_init(void) {
	uart_set_baud(UART_BUS_ID(2), 9600);
	uart_init(UART_BUS_ID(2), true);
	uart_set_r_cb(UART_BUS_ID(2), temperature_uart_r_int_handler);

	// LOG("");
}


static void temperature_oneloop(void) {
	tool_sleep_m(1);

	static u64 now = 0;
	if (now == g_time_wait_2s)
		return;
	now = g_time_wait_2s;

	temperature_write_get_req();
}


static void temperature_exit(void) {}



static void* temperature(const c8* arg) {
	unused(arg);

	temperature_init();
	while (1)
		temperature_oneloop();
	temperature_exit();

	return NULL;
}


void temperature_entry(void) {
	osThreadAttr_t attr = {
		.name = "temperature",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)temperature, NULL, &attr)) {
		ERROR("Failed to create temperature sub_thread");
	}
}

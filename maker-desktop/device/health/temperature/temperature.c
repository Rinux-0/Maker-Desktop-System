#include "temperature.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"



static u8 str_temperature[7];
static bool is_wating;


static void temperature_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	const c8* buff = (const c8*)buffer;
	s16 data = atoi(buff);

	sprintf((c8*)str_temperature, "t%+05.1f", data / 10.0f);
	sle_write(receiver, str_temperature, sizeof(str_temperature) - 1);

	is_wating = false;

	DATA("\n\ttemp: %+5.1f\n\n", data / 10.0f);

	/// @todo WIFI 发给上位机
	// DATA("\n\ttemp: %-d.%d\n\n", data % 1000 / 10, abs(data) % 10);
}


static void temperature_write_get_req(void) {
	const u8 data[] = "\xA1\xAB";

	uart_write(UART_BUS_ID(2), data, sizeof(data) - 1);
	is_wating = true;

	while (is_wating)
		tool_delay_m(10);

	// LOG("");
}


void temperature_init(void) {
	uart_set_baud(UART_BUS_ID(2), 9600);
	uart_init(UART_BUS_ID(2), true);
	uart_set_r_cb(UART_BUS_ID(2), temperature_uart_r_int_handler);

	// LOG("");
}


void temperature_oneloop(void) {
	temperature_write_get_req();
}


void temperature_exit(void) {}

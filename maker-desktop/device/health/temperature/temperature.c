#include "temperature.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"



static void temperature_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	const su8* buff = (const su8*)buffer;
	s16 data = atoi(buff);

	LOG("");
	/// @todo WIFI 发给上位机
	DATA("\n\ttemp: %-d.%d\n\n", data % 1000 / 10, abs(data) % 10);
}


static void temperature_write_get_req(void) {
	const u8 data[] = "\xA1\xAB";

	uart_write(2, data, sizeof(data) - 1);

	// LOG("");
}


void temperature_init(void) {
	uart_init(2, 9600, true);
	uart_set_r_cb(2, temperature_uart_r_int_handler);

	// LOG("");
}


void temperature_oneloop(void) {
	temperature_write_get_req();
}


void temperature_exit(void) {}

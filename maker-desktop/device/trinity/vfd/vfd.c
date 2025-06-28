#include "vfd.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"



static void vfd_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	LOG("\n\tlength: %d\n\tbuffer: %s\n\n", length, (c8*)buffer);

	uart_write(UART_BUS_ID(1), (u8*)buffer, length);
}


void vfd_init(void) {
	uart_set_baud(UART_BUS_ID(1), 9600);
	uart_set_parity(UART_BUS_ID(1), 1);
	uart_init(UART_BUS_ID(1), true);

	uart_set_r_cb(UART_BUS_ID(1), vfd_uart_r_int_handler);

	uapi_pin_set_ds(8, PIN_DS_MAX);
}


void vfd_oneloop(void) {}


void vfd_exit(void) {
	LOG("vfd_exit\n");
}

#include "vfd.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"

#include <cmsis_os2.h>



static void vfd_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	LOG("\n\tlength: %d\n\tbuffer: %s\n\n", length, (c8*)buffer);

	uart_write(UART_BUS_ID(2), (u8*)buffer, length);
}


static void vfd_init(void) {
	uart_set_baud(UART_BUS_ID(2), 9600);
	uart_set_parity(UART_BUS_ID(2), 1);
	uart_init(UART_BUS_ID(2), true);

	uart_set_r_cb(UART_BUS_ID(2), vfd_uart_r_int_handler);

	uapi_pin_set_ds(8, PIN_DS_MAX);
}


static void vfd_oneloop(void) {
	tool_delay_m(1);
}


static void vfd_exit(void) {}



static void* vfd(const c8* arg) {
	unused(arg);

	vfd_init();
	while (1)
		vfd_oneloop();
	vfd_exit();

	return NULL;
}


void vfd_entry(void) {
	osThreadAttr_t attr = {
		.name = "vfd",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)vfd, NULL, &attr)) {
		ERROR("Failed to create vfd sub_thread");
	}
}

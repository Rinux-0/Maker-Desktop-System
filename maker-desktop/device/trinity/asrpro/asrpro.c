#include "asrpro.h"

#include "ddef.h"
#include "ttool.h"

#include "asp_core.h"
#include "uuart.h"

#include <cmsis_os2.h>



static void asrpro_uart_r_cb(const void* buffer, u16 length, bool error) {
	unused(error);

	asp_cmd_entry((c8*)buffer, length);
}


static void asrpro_init(void) {
	asp_core_init_pin();

	uart_set_r_cb(UART_BUS_ID(1), asrpro_uart_r_cb);
}


static void asrpro_oneloop(void) {
	tool_sleep_m(1);
}


static void asrpro_exit(void) {}



static void* asrpro(const c8* arg) {
	unused(arg);

	asrpro_init();
	while (1)
		asrpro_oneloop();
	asrpro_exit();

	return NULL;
}


void asrpro_entry(void) {
	osThreadAttr_t attr = {
		.name = "asrpro",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)asrpro, NULL, &attr)) {
		ERROR("Failed to create asrpro sub_thread");
	}
}

#include "lamp.h"

#include "ddef.h"
#include "ttool.h"

#include "core/lamp_core.h"
#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



static void lamp_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	lamp_cmd_entry((c8*)buffer, length);
}


static void lamp_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	unused(cs_id);
	unused(conn_id);
	unused(status);

	lamp_cmd_entry((c8*)read_cb_para->value, read_cb_para->length);
}


static void lamp_init(void) {
	sle_set_r_cb(lamp_sle_r_int_handler);
	uart_set_r_cb(UART_BUS_ID(0), lamp_uart_r_int_handler);

	lamp_core_init_pin();
}


static void lamp_oneloop(void) {
	tool_sleep_m(1);
}


static void lamp_exit(void) {}



static void* lamp(const c8* arg) {
	unused(arg);

	lamp_init();
	while (1)
		lamp_oneloop();
	lamp_exit();

	return NULL;
}


void lamp_entry(void) {
	osThreadAttr_t attr = {
		.name = "lamp",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)lamp, NULL, &attr)) {
		ERROR("Failed to create lamp sub_thread");
	}
}

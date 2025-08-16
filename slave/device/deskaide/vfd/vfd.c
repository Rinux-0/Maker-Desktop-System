#include "vfd.h"

#include "ddef.h"
#include "ttool.h"

#include "vfd_def.h"
#include "core/vfd_core.h"
#include "mode/mode.h"

#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



/// @note 该函数未被注册
/*static*/ void vfd_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	LOG("\n\tlength: %d\n\tbuffer: %s\n\n", length, (c8*)buffer);

	vfd_core_set_char(*(c8*)buffer, 1, 1, true);
}


void vfd_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	unused(cs_id);
	unused(conn_id);
	unused(status);

	c8* tmp;
	c8* sub_str = (c8*)(read_cb_para->value);
	u16 sub_len = read_cb_para->length;

	if ((tmp = strnstr(sub_str, "sle_conn_array", sub_len))) {		// sle_conn_array
		DATA("sle_conn_array ");
		sub_str = sizeof("sle_conn_array") + tmp;
		sub_len -= sizeof("sle_conn_array");

		for (u8 i = 0; i < 4; i++)
			vfd_update_sle_conn_status(i, sub_str[i * 2 + 4] != '-');
	} else {
		vfd_cmd_entry(sub_str, sub_len);
	}
}


static void vfd_gpio_r_int_handler(pin_t pin, uintptr_t param) {
	unused(pin);
	unused(param);

	vfd_set_mode_next();
}


static void vfd_show(void) {
	switch (vfd_get_mode()) {
	default:					vfd_set_mode(VFD_MODE_OFF);
	/****/case VFD_MODE_OFF:	vfd_core_clear_screen();
	break;case VFD_MODE_CHARGE:	vfd_show_mode_charge();
	break;case VFD_MODE_CONN:	vfd_show_mode_conn();
	}
}


static void vfd_init(void) {
	// GPIO中断 设置
	uapi_pin_set_mode(VFD_GPIO_INT_PIN, 0);
	uapi_gpio_set_dir(VFD_GPIO_INT_PIN, GPIO_DIRECTION_INPUT);
	uapi_gpio_register_isr_func(VFD_GPIO_INT_PIN, GPIO_INTERRUPT_DEDGE, vfd_gpio_r_int_handler);

	uapi_gpio_enable_interrupt(VFD_GPIO_INT_PIN);

	uart_set_baud(UART_BUS_ID(2), 9600);
	uart_set_parity(UART_BUS_ID(2), 1);
	uart_init(UART_BUS_ID(2), true);

	// uart_set_r_cb(UART_BUS_ID(2), vfd_uart_r_int_handler);

	uapi_pin_set_ds(8, PIN_DS_MAX);

	tool_timer_start_m(3, 1000 * 0.5, NULL);
}


static void vfd_oneloop(void) {
	tool_sleep_m(1);

	static bool now = false;
	if (now == g_time_wait_0s5)
		return;
	now = g_time_wait_0s5;

	// vfd_core_clear_screen();

	vfd_show();

	vfd_core_set_pos(1, 1);
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

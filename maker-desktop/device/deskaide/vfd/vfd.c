#include "vfd.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle_def.h"
#include "uuart.h"
#include "core/vfd_core.h"

#include <cmsis_os2.h>



static s8 vfd_sle_conn_status[4] = { -1, -1, -1, -1 };



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

		for (u8 i = 0; i < 4; i++) {
			vfd_sle_conn_status[i] = (sub_str[i * 2 + 4] == '-')
				? '\351' : '\333';
		}
	} else {
		vfd_cmd_entry(sub_str, sub_len);
	}
}


static void vfd_init(void) {
	uart_set_baud(UART_BUS_ID(2), 9600);
	uart_set_parity(UART_BUS_ID(2), 1);
	uart_init(UART_BUS_ID(2), true);

	uart_set_r_cb(UART_BUS_ID(2), vfd_uart_r_int_handler);

	uapi_pin_set_ds(8, PIN_DS_MAX);
}


static void vfd_oneloop(void) {
	tool_sleep_m(1);

	static u64 now = 0;
	if (now != g_time_wait_2s) {
		now = g_time_wait_2s;

		u8* vfd_l1 = (u8*)"Maker Desktop System";
		u8 vfd_l2[21];
		if (sle_is_connected()) {
			u8 target_id = deskaide;
			sle_write(receiver, &target_id, 1);		// 请求 sle_conn_status
			sprintf((c8*)vfd_l2, "dnm%c hlth%c kbd%c kpd%c",
				vfd_sle_conn_status[0], vfd_sle_conn_status[1],
				vfd_sle_conn_status[2], vfd_sle_conn_status[3]
			);
		} else {
			strcpy((c8*)vfd_l2, "   disconnected...  ");
		}

		vfd_core_set_screen(vfd_l1, vfd_l2, true);
		vfd_core_set_pos(1, 1);
	}
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

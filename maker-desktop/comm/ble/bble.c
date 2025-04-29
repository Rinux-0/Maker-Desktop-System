#include "bble.h"
#include "bble_def.h"

#include "ddef.h"
#include "ttool.h"

#include "bble_server_core.h"
#include "uuart.h"

#include <bts_gatt_server.h>
#include <securec.h>



unsigned long mouse_msg_queue = 0;
u32 msg_rev_size = sizeof(msg_data_t);



void ble_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);
	LOG("ble_uart_r_int_handler server.\n");

	msg_data_t msg_data = {};
	void* buffer_cpy = osal_vmalloc(length);

	if (ble_uart_get_connection_state() != 0) {
		if (memcpy_s(buffer_cpy, length, buffer, length) != EOK) {
			osal_vfree(buffer_cpy);
			return;
		}

		msg_data.value = (uint8_t*)buffer_cpy;
		msg_data.value_len = length;

		osal_msg_queue_write_copy(mouse_msg_queue, (void*)&msg_data, msg_rev_size, 0);
	}
}


void ble_init(void) {
	ble_uart_server_init();
}


void ble_oneloop(void) {
	msg_data_t msg_data = {};

	u8 msg_ret = osal_msg_queue_read_copy(
		mouse_msg_queue,
		&msg_data,
		&msg_rev_size,
		3000	// OSAL_WAIT_FOREVER
	);

	if (msg_ret != OSAL_SUCCESS) {
		LOG("msg queue read copy fail.");
		if (msg_data.value != NULL) {
			osal_vfree(msg_data.value);
		}
		return;
	}

	if (msg_data.value != NULL) {
		ble_uart_server_send_input_report(msg_data.value, msg_data.value_len);
		osal_vfree(msg_data.value);
	}
}


void ble_exit(void) {}

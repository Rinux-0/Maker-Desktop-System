#include "ssle_core_server.h"
#include "ssle_core_server_adv.h"
#include "ssle_server.h"
#include "../ssle_def.h"

#include "def.h"
#include "pinctrl.h"
#include "securec.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_low_latency.h"
#include "sle_ssap_server.h"
#include "soc_osal.h"
#include "tool.h"
#include "uuart.h"



unsigned long g_sle_server_msgqueue_id;



static void ssaps_server_read_request_cbk(u8 server_id, u16 conn_id, ssaps_req_read_cb_t* read_cb_para,
	errcode_t status) {
	osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
}


static void ssaps_server_write_request_cbk(u8 server_id, u16 conn_id, ssaps_req_write_cb_t* write_cb_para,
	errcode_t status) {
	osal_printk("%s ssaps write request callback cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, conn_id, write_cb_para->handle, status);
	if ((write_cb_para->length > 0) && write_cb_para->value) {
		osal_printk("\n sle recived data : %s\r\n", write_cb_para->value);
		uart_write((u8*)write_cb_para->value, write_cb_para->length);
	}
}


static void sle_server_read_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);
	if (sle_client_is_connected()) {
		sle_server_send_report_by_handle(buffer, length);
	} else {
		osal_printk("%s sle client is not connected! \r\n", SLE_SERVER_LOG);
	}
}


static void sle_server_create_msgqueue(void) {
	if (osal_msg_queue_create("sle_server_msgqueue", SLE_SERVER_MSG_QUEUE_LEN, \
		(unsigned long*)&g_sle_server_msgqueue_id, 0, SLE_SERVER_MSG_QUEUE_MAX_SIZE) != OSAL_SUCCESS) {
		osal_printk("^%s sle_server_create_msgqueue message queue create failed!\n", SLE_SERVER_LOG);
	}
}


static void sle_server_delete_msgqueue(void) {
	osal_msg_queue_delete(g_sle_server_msgqueue_id);
}


static void sle_server_write_msgqueue(u8* buffer_addr, u16 buffer_size) {
	osal_msg_queue_write_copy(g_sle_server_msgqueue_id, (void*)buffer_addr, (u32)buffer_size, 0);
}


static s32 sle_server_receive_msgqueue(u8* buffer_addr, u32* buffer_size) {
	return osal_msg_queue_read_copy(g_sle_server_msgqueue_id, (void*)buffer_addr, buffer_size, SLE_SERVER_QUEUE_DELAY);
}


static void sle_server_rx_buf_init(u8* buffer_addr, u32* buffer_size) {
	*buffer_size = SLE_SERVER_MSG_QUEUE_MAX_SIZE;
	(void)memset_s(buffer_addr, *buffer_size, 0, *buffer_size);
}


void* sle_server_task(const char* arg) {
	unused(arg);
	u8 rx_buf[SLE_SERVER_MSG_QUEUE_MAX_SIZE] = { 0 };
	u32 rx_length = SLE_SERVER_MSG_QUEUE_MAX_SIZE;
	u8 sle_connect_state[] = "sle_dis_connect";

	sle_server_create_msgqueue();
	sle_server_register_msg(sle_server_write_msgqueue);
	sle_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);

	errcode_t ret = uart_set_rx_cb(sle_server_read_int_handler);
	if (ret != ERRCODE_SUCC) {
		osal_printk("%s Register uart callback fail.[%x]\r\n", SLE_SERVER_LOG, ret);
		return NULL;
	}

	while (1) {
		sle_server_rx_buf_init(rx_buf, &rx_length);
		sle_server_receive_msgqueue(rx_buf, &rx_length);
		if (strncmp((const char*)rx_buf, (const char*)sle_connect_state, sizeof(sle_connect_state)) == 0) {
			ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
			if (ret != ERRCODE_SLE_SUCCESS) {
				osal_printk("%s sle_connect_state_changed_cbk, sle_start_announce fail :%02x\r\n", SLE_SERVER_LOG, ret);
			}
		}
		osal_msleep(SLE_TASK_DURATION_MS);
	}
	sle_server_delete_msgqueue();
	return NULL;
}

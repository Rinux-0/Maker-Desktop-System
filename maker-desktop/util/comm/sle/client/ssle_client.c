#include "ssle_client.h"
#include "ssle_core_client.h"
#include "../ssle_def.h"

#include "def.h"
#include "pinctrl.h"
#include "sle_connection_manager.h"
#include "sle_low_latency.h"
#include "sle_ssap_client.h"
#include "soc_osal.h"
#include "tool.h"
#include "uuart.h"



// uart -> 我 -> sle_server
static void sle_client_read_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;
	char num_str[2] = { buff[0], '\0' };      // 首字符作conn_id
	ssapc_write_param_t* sle_send_param = get_sle_client_send_param();
	u16 g_sle_conn_id = atoi(num_str);

	osal_printk("\n sle_client_read_int_handler: %d\r\n", g_sle_conn_id);
	sle_send_param->data_len = length - 1;
	sle_send_param->data = (u8*)buffer + 1;
	ssapc_write_req(0, g_sle_conn_id, sle_send_param);		// sle 发送数据
}


void* sle_client_task(const char* arg) {
	unused(arg);

	sle_client_init();

	uart_set_rx_cb(sle_client_read_int_handler);

	return NULL;
}

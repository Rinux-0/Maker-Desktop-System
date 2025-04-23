#include "ssle.h"
#include "ssle_def.h"

#include "ddef.h"
#include "ttool.h"

#include "client/ssle_client_core.h"

#include <sle_device_discovery.h>
#include <sle_ssap_client.h>



errcode_t sle_set_r_cb(sle_r_cb_t cb) {
	LOG("[SLE Client] set r cb.\n");

	return sle_client_set_r_cb(cb);
}


void sle_init(void) {
	sle_client_core_init();

	if (enable_sle() != ERRCODE_SUCC)
		ERROR("[SLE Client] sle enbale fail !\n");
}


void sle_oneloop(void) {}


void sle_exit(void) {}


/// @param[in] conn_id 发送对象(server)的连接ID
void sle_write(u8 conn_id, const u8* data, u32 length) {
	ssapc_write_param_t* sle_send_param = sle_client_get_send_param();

	sle_send_param->data_len = length;
	sle_send_param->data = (u8*)data;

	// sle 发送数据
	if (ERRCODE_SUCC == ssapc_write_req(0, conn_id, sle_send_param)) {
		LOG("[SLE Clinet] send report success !\n");
	} else {
		LOG("[SLE Clinet] send report fail !\n");
	}
}


void sle_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;
	u16 sle_conn_id = atoi((const char[2]) { buff[0], '\0' });

	LOG("\n sle_uart_client_read_int_handler: %d\r\n", sle_conn_id);
	sle_write(0, buff+1, length-1);
}

#include "ssle.h"
#include "ssle_def.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle_server_adv.h"
#include "ssle_server_core.h"
#include "ssle_server_msgqueue.h"

#include "uuart.h"

#include <sle_device_discovery.h>
#include <sle_errcode.h>
#include <sle_ssap_server.h>



// static volatile bool is_wating;
u8 rx_buf[SLE_SERVER_MSG_QUEUE_MAX_SIZE] = { 0 };
u32 rx_length = SLE_SERVER_MSG_QUEUE_MAX_SIZE;
u8 sle_connect_state[] = "sle_dis_connect";



errcode_t sle_set_r_cb(sle_r_cb_t cb) {
	return sle_server_set_r_cb(cb);
}


void sle_init(void) {
	tool_tcxo_init();

	sle_server_msgqueue_create();
	sle_server_register_msg(sle_server_msgqueue_write);

	if (enable_sle() != ERRCODE_SUCC)
		ERROR("[SLE Server] sle enbale fail !\n");

	sle_server_init_core();
	sle_server_adv_init();

	LOG("");
}


void sle_oneloop(void) {
	static u64 start_time_ms = 0;

	if (!tool_tcxo_is_timeout(3000, start_time_ms))
		return;

	// reinit
	rx_length = SLE_SERVER_MSG_QUEUE_MAX_SIZE;
	(void)memset_s(rx_buf, SLE_SERVER_MSG_QUEUE_MAX_SIZE, 0, SLE_SERVER_MSG_QUEUE_MAX_SIZE);

	sle_server_msgqueue_receive(rx_buf, &rx_length);

	if (strncmp((const c8*)rx_buf, (const c8*)sle_connect_state, sizeof(sle_connect_state)) == 0) {
		errcode_t ret = sle_start_announce(SLE_ADV_HANDLE);
		if (ret != ERRCODE_SLE_SUCCESS) {
			ERROR("%s sle_connect_state_changed_cbk, sle_start_announce fail :%02x\n", SLE_SERVER_LOG, ret);
		}
	}

	start_time_ms = tool_tcxo_get_m();
}


void sle_exit(void) {
	sle_server_msgqueue_delete();
}


void sle_write(sle_target_t target_id, const u8* data, u32 length) {
	// if (is_wating)
	// 	return;

	// is_wating = true;

	// sle 发送数据
	if (ERRCODE_SUCC != sle_server_send_report_by_hdl(target_id, data, length))
		ERROR("[SLE Server] send report fail !\n");

	// is_wating = false;
}


// void uart_r_int_handler(const void* buffer, u16 length, bool error) {
// 	unused(error);

// 	const u8* buff = (const u8*)buffer;

// 	sle_write(buff[0], buff + 1, length - 1);
// }

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
	sle_client_init_core();

	if (enable_sle() != ERRCODE_SUCC)
		ERROR("[SLE Client] sle enbale fail !\n");
}


void sle_oneloop(void) {}


void sle_exit(void) {}


void sle_write(sle_target_t target_id, const u8* data, u32 length) {
	if (target_id >= sle_target_max) {
		ERROR("[SLE Client] target_id is invalid !\n");
		return;
	}

	s8 conn_id = sle_client_conn_id[target_id];

	if (conn_id == -1) {
		ERROR("[SLE Client] conn_id is invalid now !\n");
		return;
	}

	ssapc_write_param_t* sle_send_param = sle_client_get_send_param();

	sle_send_param->data_len = length;
	sle_send_param->data = (u8*)data;

	// sle 发送数据
	if (ERRCODE_SUCC != ssapc_write_req(0, conn_id, sle_send_param))
		ERROR("[SLE Clinet] send report fail !\n");
}

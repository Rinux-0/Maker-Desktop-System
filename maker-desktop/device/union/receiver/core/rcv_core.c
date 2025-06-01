#include "rcv_core.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#include "ssle.h"



void rcv_init_pin(void) {}


static void rcv_sle_r_int_sle_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	// sle_write(/* 某 sle_server */read_cb_para->data[0], read_cb_para->data + 1, read_cb_para->data_len - 1);
}


static void rcv_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	LOG("comm_way:%d, client_id:%d, conn_id:%d\n", comm_way, cs_id, conn_id);

	// switch (/* 转发对象 */) {
	// default:
	// break;case /* PC */:				/**/
	sle_r_int_uart_handler(cs_id, conn_id, read_cb_para, status);
	// break;case /* 某 sle_server */:		rcv_sle_r_int_sle_handler(cs_id, conn_id, read_cb_para, status);
	// }
}


void rcv_init_int_cb(void) {
	sle_set_r_cb(rcv_sle_r_int_handler);
}

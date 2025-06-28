#include "rcv_core.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#include "ssle.h"
// #include "uuart.h"		// tmp!!!
#include "wwifi.h"



void rcv_init_pin(void) {}

static void rcv_sle_r_int_sle_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	unused(cs_id);
	unused(conn_id);
	unused(status);

	LOG("");

	sle_write(read_cb_para->data[1], read_cb_para->data + 2, read_cb_para->data_len - 2);
}

static void rcv_sle_r_int_rcv_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	unused(cs_id);
	unused(conn_id);
	unused(status);

	LOG("\n\tlength: %d\n\n", read_cb_para->data_len);


	u8* d = read_cb_para->data;

	for (u8 i = 0; i < 3; i++)
		DATA("\n\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
			d[i * 16 + 0], d[i * 16 + 1], d[i * 16 + 2], d[i * 16 + 3], d[i * 16 + 4], d[i * 16 + 5], d[i * 16 + 6], d[i * 16 + 7], d[i * 16 + 8], d[i * 16 + 9], d[i * 16 + 10], d[i * 16 + 11], d[i * 16 + 12], d[i * 16 + 13], d[i * 16 + 14], d[i * 16 + 15]
		);

	/// @todo 其他本地处理

	// wifi_write(read_cb_para->data, read_cb_para->data_len);
}

static void rcv_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	if (read_cb_para->data[0] != '@' ||
		read_cb_para->data[1] < 0 ||
		read_cb_para->data[1] > sle_target_max
	) {
		ERROR("unknown object: %c[%d]\n", read_cb_para->data[0], read_cb_para->data[1]);
		return;
	}

	// LOG("target:%d, comm_way:%d, client_id:%d, conn_id:%d\n",
	// 	read_cb_para->data[1], comm_way, cs_id, conn_id
	// );

	switch (read_cb_para->data[1]) {
	default:
		rcv_sle_r_int_sle_handler(cs_id, conn_id, read_cb_para, status);
	break;case pc:
		read_cb_para->data += 2;
		read_cb_para->data_len -= 2;
		sle_r_int_uart_handler(cs_id, conn_id, read_cb_para, status);
	break;case receiver:
		read_cb_para->data += 2;
		read_cb_para->data_len -= 2;
		rcv_sle_r_int_rcv_handler(cs_id, conn_id, read_cb_para, status);
	}
}

void rcv_init_int_cb(void) {
	sle_set_r_cb(rcv_sle_r_int_handler);
	// uart_set_r_cb( ... );
}

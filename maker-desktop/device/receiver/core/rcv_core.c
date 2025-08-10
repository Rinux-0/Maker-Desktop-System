#include "rcv_core.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#include "ssle.h"
// #include "uuart.h"		// tmp!!!
#include "wwifi.h"



void rcv_init_pin(void) {}

static void rcv_to_host(u8* raw_data, u16 raw_len) {
	c8 str_data[256];
	c8 pure_data[20];

	u8 cmd_id = raw_data[0];
	raw_data += 1;
	raw_len -= 1;

	sprintf(pure_data, "%s", raw_data);
	pure_data[raw_len] = '\0';

	switch (cmd_id) {
	default:	ERROR("");
	break;case 'd':
		sprintf(str_data, "/health/?data_type=distance&data=%s", pure_data);
		wifi_write("POST", str_data, false, "");
	break;case 'h':
		sprintf(str_data, "/health/?data_type=heart&data=%s", pure_data);
		wifi_write("POST", str_data, false, "");
	break;case 'b':
		sprintf(str_data, "/health/?data_type=breath&data=%s", pure_data);
		wifi_write("POST", str_data, false, "");
	break;case 't':
		sprintf(str_data, "/health/?data_type=temperature&data=%s", pure_data);
		wifi_write("POST", str_data, false, "");
	break;case 'f':
		sprintf(str_data, "/user/whoami?user_id=%s", pure_data);
		for (u8 i = 0; i < 5; i++) {
			wifi_write("POST", str_data, false, "");
			tool_sleep_m(10);
		}
	break;case 'n':
		cmd_id = raw_data[0];
		raw_data += 1;
		raw_len -= 1;

		if (cmd_id == 'b') {
			raw_data[16 - 1] = raw_data[16 * 2 - 1] = raw_data[16 * 3 - 1] = '\0';

			// sprintf(str_data, "{\"name\": \"%s\", \"age\": %u, \"height\": %u, \"weight\": %u, \"blood\": \"%c\", \"phone\": \"%s\", \"intro\": \"%s\"}",
			// 	(c8*)raw_data, raw_data[16], raw_data[16 + 1], raw_data[16 + 2], raw_data[16 + 3], (c8*)&(raw_data[16 + 4]), (c8*)&(raw_data[16 * 2])
			// );

			// c8* p = strnstr(str_data, "{\"", 8);
			// wifi_write("POST", "/user/", true, p);

			sprintf(str_data, "/user/?name=%s&age=%u&height=%u&weight=%u&blood=%c&phone=%s&intro=%s",
				(c8*)raw_data, raw_data[16], raw_data[16 + 1], raw_data[16 + 2], raw_data[16 + 3], (c8*)&(raw_data[16 + 4]), (c8*)&(raw_data[16 * 2])
			);

			wifi_write("POST", str_data, false, "");
		}
	}

	// DATA("\n\t");
	// DATA(p);
	// DATA("\n\n");
}


void rcv_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
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

	u8 target_id = read_cb_para->data[1];

	read_cb_para->data += 2;
	read_cb_para->data_len -= 2;
	u8* d = read_cb_para->data;
	switch (target_id) {
	default:
		sle_write(target_id, read_cb_para->data, read_cb_para->data_len);
		for (u8 i = 0; i < 1; i++)
			DATA("\n\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%s\n\n",
				d[i * 16 + 0], d[i * 16 + 1], d[i * 16 + 2], d[i * 16 + 3],
				d[i * 16 + 4], d[i * 16 + 5], d[i * 16 + 6], d[i * 16 + 7],
				d[i * 16 + 8], d[i * 16 + 9], d[i * 16 + 10], d[i * 16 + 11],
				d[i * 16 + 12], d[i * 16 + 13], d[i * 16 + 14], d[i * 16 + 15],
				(c8*)d
			);
	break;case pc:
		// for (u8 i = 0; i < 1; i++)
		// 	DATA("\n\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
		// 		d[i * 16 + 0], d[i * 16 + 1], d[i * 16 + 2], d[i * 16 + 3],
		// 		d[i * 16 + 4], d[i * 16 + 5], d[i * 16 + 6], d[i * 16 + 7],
		// 		d[i * 16 + 8], d[i * 16 + 9], d[i * 16 + 10], d[i * 16 + 11],
		// 		d[i * 16 + 12], d[i * 16 + 13], d[i * 16 + 14], d[i * 16 + 15]
		// 	);
		if (read_cb_para->data[0] == 0x57 &&
			read_cb_para->data[1] == 0xAB &&
			read_cb_para->data[2] == 0x00
		) {			// CH9329
			// u8* d = read_cb_para->data;
			// for (u8 i = 0; i < 1; i++)
			// 	DATA("\n\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
			// 		d[i * 16 + 0], d[i * 16 + 1], d[i * 16 + 2], d[i * 16 + 3],
			// 		d[i * 16 + 4], d[i * 16 + 5], d[i * 16 + 6], d[i * 16 + 7],
			// 		d[i * 16 + 8], d[i * 16 + 9], d[i * 16 + 10], d[i * 16 + 11],
			// 		d[i * 16 + 12], d[i * 16 + 13], d[i * 16 + 14], d[i * 16 + 15]
			// 	);
			sle_r_int_uart_handler(cs_id, conn_id, read_cb_para, status);
		} else {	// 上位机
			rcv_to_host(read_cb_para->data, read_cb_para->data_len);
			// LOG("");

			// if (read_cb_para->data[0]) {
			// 	u8* d = read_cb_para->data;
			// 	DATA("\n\t[%c%c]", d[0], d[1]);
			// 	d += 2;
			// 	for (u8 i = 0; i < 3; i++)
			// 		DATA("\n\t%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n",
			// 			d[i * 16 + 0], d[i * 16 + 1], d[i * 16 + 2], d[i * 16 + 3],
			// 			d[i * 16 + 4], d[i * 16 + 5], d[i * 16 + 6], d[i * 16 + 7],
			// 			d[i * 16 + 8], d[i * 16 + 9], d[i * 16 + 10], d[i * 16 + 11],
			// 			d[i * 16 + 12], d[i * 16 + 13], d[i * 16 + 14], d[i * 16 + 15]
			// 		);
			// }
		}
	break;case receiver:
		sle_write_conn_id_array(read_cb_para->data[0]);
	}
}

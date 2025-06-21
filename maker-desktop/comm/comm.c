#include "comm.h"
#include "comm_def.h"

#include "ddef.h"
#include "ttool.h"

#if defined(CONFIG_COMM_I2C)
#	include "ii2c.h"
#endif
#if defined(CONFIG_COMM_SPI)
#	include "sspi.h"
#endif
#if defined(CONFIG_COMM_UART)
#	include "uuart.h"
#endif
#if defined(CONFIG_COMM_SLE)
#	include "ssle.h"
#endif
#if defined(CONFIG_COMM_WIFI)
#	include "wwifi.h"
#endif



comm_way_t comm_way = COMM_WAY_NONE;
comm_way_t comm_way_past = COMM_WAY_NONE;


/// @todo union, lamp
/// @note [0, 1, 2]
const bool uart_need_inited[3] = {
	 CONFIG_COMM_UART_BUS_ID / 0100 % 01000,
	 CONFIG_COMM_UART_BUS_ID / 0010 % 00100,
	 CONFIG_COMM_UART_BUS_ID / 0001 % 00010
};



/* 多选 */
void comm_init(void) {
	i2c_init();
	spi_init();
	sle_init();
	wifi__init();

	for (u8 id_init = 0; id_init < 3; id_init++)
		if (uart_need_inited[id_init])
			uart_init(id_init, 0, false);

	LOG("");
}


/* 多选 */
void comm_oneloop(void) {
	if (comm_way != comm_way_past) {
		comm_way_past = comm_way;
		/// @todo 通讯方式
	}

	i2c_oneloop();
	spi_oneloop();
	sle_oneloop();
	wifi_oneloop();

	for (u8 id_oneloop = 0; id_oneloop < 3; id_oneloop++)
		if (uart_need_inited[id_oneloop])
			uart_oneloop(id_oneloop);
}


/* 多选 */
void comm_exit(void) {
	i2c_exit();
	spi_exit();
	sle_exit();
	wifi_exit();

	for (u8 id_exit = 0; id_exit < 3; id_exit++)
		if (uart_need_inited[id_exit])
			uart_exit(id_exit);
}



/// @note“通信方式转换”回调
#if defined(CONFIG_COMM_UART) && defined(CONFIG_COMM_SLE)
/// @note uart_bus_id: 固定为1
void sle_r_int_uart_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	if (!uart_is_inited[1])
		return;

	LOG("cs_id=%d, conn_id=%d, status=%d\n", cs_id, conn_id, status);

#	if defined(CONFIG_COMM_SLE_CLIENT)
	uart_write(UART_BUS_ID(1), read_cb_para->data, read_cb_para->data_len);
#	elif defined(CONFIG_COMM_SLE_SERVER)
	uart_write(UART_BUS_ID(2), read_cb_para->value, read_cb_para->length);		/// @todo UART_BUS_ID 待修改,不一定是 2
#	endif
}


void uart_r_int_sle_handler(const void* buffer, u16 length, bool error) {
	unused(error);
	LOG("uart_r_int_handler\n\terror=%d\n", error);

#	if defined(CONFIG_COMM_SLE_CLIENT)
	u8* buff = (u8*)buffer;
	u16 sle_conn_id = atoi((const char[2]) { buff[0], '\0' });

	LOG("\tsle_conn_id: %d\n", sle_conn_id);

	sle_write(sle_conn_id, buff + 1, length - 1);
#	elif defined(CONFIG_COMM_SLE_SERVER)
	sle_write(0, (const u8*)buffer, length);
#	endif
}
#endif	// CONFIG_COMM_UART && CONFIG_COMM_SLE

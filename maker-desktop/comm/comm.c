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
// comm_way_t comm_way_past = COMM_WAY_NONE;

/// @note [0, 1]
const bool spi_need_inited[2] = {
	COMM_SPI_BUS_ID / 010 % 0100,
	COMM_SPI_BUS_ID / 001 % 0010
};

/// @note [0, 1, 2]
const bool uart_need_inited[3] = {
	COMM_UART_BUS_ID / 0100 % 01000,
	COMM_UART_BUS_ID / 0010 % 00100,
	COMM_UART_BUS_ID / 0001 % 00010
};



/* 多选 */
/*static */void comm_init(void) {
	wifi_entry();

	i2c_init();
	sle_init();
	// wifi__init();

	for (u8 id = 0; id < 2; id++)
		if (spi_need_inited[id]) {
			spi_init(id);
		}

	for (u8 id = 0; id < 3; id++)
		if (uart_need_inited[id]) {
			uart_init(id, false);
		}

	LOG("");
}


/* 多选 */
/*static */void comm_oneloop(void) {
	// tool_sleep_m(1);

	// if (comm_way != comm_way_past) {
	// 	comm_way_past = comm_way;
	// 	/// @todo 通讯方式
	// }

	// i2c_oneloop();
	sle_oneloop();
	// wifi_oneloop();

	// for (u8 id = 0; id < 2; id++)
	// 	if (spi_need_inited[id])
	// 		spi_oneloop(id);

	// for (u8 id = 0; id < 3; id++)
	// 	if (uart_need_inited[id])
	// 		uart_oneloop(id);

	// DATA("%d", COMM_SPI_BUS_ID);
}


/* 多选 */
/*static */void comm_exit(void) {
	// i2c_exit();
	sle_exit();
	// wifi_exit();

	// for (u8 id = 0; id < 2; id++)
	// 	if (spi_need_inited[id])
	// 		spi_exit(id);

	for (u8 id = 0; id < 3; id++)
		if (uart_need_inited[id])
			uart_exit(id);
}


// static void* comm(const c8* arg) {
// 	unused(arg);

// 	comm_init();
// 	while (1)
// 		comm_oneloop();
// 	comm_exit();

// 	return NULL;
// }


// void comm_entry(void) {
// 	osThreadAttr_t attr = {
// 		.name = "comm",
// 		.attr_bits = 0U,
// 		.cb_mem = NULL,
// 		.cb_size = 0U,
// 		.stack_mem = NULL,
// 		.stack_size = 0x,
// 		.priority = (osPriority_t)17,
// 		// .tz_module	= 0U,
// 		// .reserved	= 0U
// 	};

// 	if (NULL == osThreadNew((osThreadFunc_t)comm, NULL, &attr)) {
// 		ERROR("Failed to create comm sub_thread");
// 	}
// }


/// @note“通信方式转换”回调
#if defined(CONFIG_COMM_UART) && defined(CONFIG_COMM_SLE)
void sle_r_int_uart_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	/// @todo uart_bus_id 改为可指定的

	if (!uart_is_inited[1])
		return;

	// LOG("cs_id=%d, conn	_id=%d, status=%d\n", cs_id, conn_id, status);

#	if defined(CONFIG_COMM_SLE_CLIENT)
	u8* d = read_cb_para->data;
	DATA("\n\t%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n\n",
		d[0], d[1], d[2], d[3], d[4], d[5], d[6],
		d[7], d[8], d[9], d[10], d[11], d[12], d[13]
	);

	uart_write(UART_BUS_ID(1), read_cb_para->data, read_cb_para->data_len);
#	elif defined(CONFIG_COMM_SLE_SERVER)	// 不用
	uart_write(UART_BUS_ID(1), read_cb_para->value, read_cb_para->length);		/// @todo UART_BUS_ID 待修改
#	endif
}


void uart_r_int_sle_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	const u8* buff = (const u8*)buffer;

	if (buff[0] != '@' || buff[1] < 0 || buff[1] > sle_target_max) {
		ERROR("unknown object: %c[%d]\n", buff[0], buff[1]);
		return;
	}

	u8 sle_target_id = buff[1];

	LOG("\nsle_target_id: %d\n", sle_target_id);

#	if defined(CONFIG_COMM_SLE_CLIENT)	// 不用
#	elif defined(CONFIG_COMM_SLE_SERVER)
#	endif

	sle_write(buff[0], buff + 2, length - 2);
}
#endif	// CONFIG_COMM_UART && CONFIG_COMM_SLE

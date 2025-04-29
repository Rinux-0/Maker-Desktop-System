#include "comm.h"
#include "comm_def.h"

#include "ddef.h"
#include "ttool.h"

#if defined(CONFIG_COMM_I2C)
#include "ii2c.h"
#endif
#if defined(CONFIG_COMM_UART)
#include "uuart.h"
#endif
#if defined(CONFIG_COMM_WIFI)
#include "wwifi.h"
#endif
#if defined(CONFIG_COMM_BLE)
#include "bble.h"
#endif
#if defined(CONFIG_COMM_SLE)
#include "ssle.h"
#endif



comm_way_t comm_w_way_past = COMM_WAY_NONE;
comm_way_t comm_w_way =
#if defined(CONFIG_HEALTH) || defined(CONFIG_DEVICE_NFC)
COMM_WAY_WIFI
#elif defined(CONFIG_DEVICE_RECEIVER)
COMM_WAY_UART
#else
COMM_WAY_SLE
#endif
;

const comm_way_t comm_r_way =
#if defined(CONFIG_HEALTH_DISTANCE)
COMM_WAY_I2C
#elif defined(CONFIG_DEVICE_RECEIVER)
COMM_WAY_SLE
#else
COMM_WAY_UART
#endif
;

static const bool uart_need_inited[3] = {
#   if defined(CONFIG_HEALTH) && !defined(CONFIG_HEALTH_FINGERPRINT)
	1, 1, 1		// distance : I2C
#	elif defined(CONFIG_DEVICE_KEYBOARD) || defined(CONFIG_DEVICE_UNION)
	1, 1, 1
#	else
	0, 1, 0
#	endif
};



/* 多选 */
void comm_init(void) {
	i2c_init();
	wifi_init();
	ble_init();
	sle_init();

	for (u8 id_init=0; id_init<3; id_init++)
		if (uart_need_inited[id_init])
			uart_init(id_init);

#	if defined(CONFIG_HEALTH_DISTANCE)
	uart_set_r_cb(UART_BUS_ID(1), health_uart_r_int_handler);
#	endif
}


/* 单选 */
void comm_oneloop(void) {
	if (comm_w_way != comm_w_way_past) {
		comm_w_way_past = comm_w_way;

		switch (comm_w_way) {	// 可能根本不需要
		default: 					ERROR("comm_w_way = %d\n", comm_w_way);
		break;case COMM_WAY_WIFI:	uart_set_r_cb(UART_BUS_ID(1), wifi_uart_r_int_handler);
		break;case COMM_WAY_BLE:	uart_set_r_cb(UART_BUS_ID(1), ble_uart_r_int_handler);
		break;case COMM_WAY_SLE:	uart_set_r_cb(UART_BUS_ID(1), sle_uart_r_int_handler);
		break;case COMM_WAY_UART:;
		}
	}

	switch (comm_w_way) {
	default: 	ERROR("comm_w_way = %d\n", comm_w_way);
	break;case COMM_WAY_I2C:		i2c_oneloop();
	break;case COMM_WAY_WIFI:		wifi_oneloop();
	break;case COMM_WAY_BLE:		ble_oneloop();
	break;case COMM_WAY_SLE:		sle_oneloop();
	break;case COMM_WAY_UART:
		for (u8 id_oneloop=0; id_oneloop<3; id_oneloop++)
			if (uart_need_inited[id_oneloop])
				uart_oneloop(id_oneloop);
	}
}


/* 多选 */
void comm_exit(void) {
	i2c_exit();
	wifi_exit();
	ble_exit();
	sle_exit();

	for (u8 id_exit=0; id_exit<3; id_exit++)
		if (uart_need_inited[id_exit])
			uart_exit(id_exit);
}

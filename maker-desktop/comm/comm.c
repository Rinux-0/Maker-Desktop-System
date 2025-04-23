#include "comm.h"
#include "comm_def.h"

#include "ddef.h"
#include "ttool.h"

#if defined(CONFIG_COMM_BLE)
#include "bble.h"
#endif
#if defined(CONFIG_COMM_I2C)
#include "ii2c.h"
#endif
#if defined(CONFIG_COMM_SLE)
#include "ssle.h"
#endif
#if defined(CONFIG_COMM_UART)
#include "uuart.h"
#endif
#if defined(CONFIG_COMM_WIFI)
#include "wwifi.h"
#endif



comm_way_t comm_way;
static const bool uart_need_inited[3] = {
#   if defined(CONFIG_HEALTH_DISTANCE)
	0, 0, 0
#	elif defined(CONFIG_DEVICE_KEYBOARD) || defined(CONFIG_DEVICE_UNION)
	1, 1, 1
#	else
	0, 1, 0
#	endif
};



/* 多选 */
void comm_init(void) {
	ble_init();
	i2c_init();
	sle_init();
	wifi_init();

	for (u8 id_init=0; id_init<3; id_init++)
		if (uart_need_inited[id_init])
			uart_init(id_init);

#	if defined(CONFIG_COMM_SLE)
	sle_set_r_cb(uart_sle_r_int_handler);
	uart_set_r_cb(UART_BUS_ID(1), sle_uart_r_int_handler);
#	elif defined(CONFIG_HEALTH) && !defined(CONFIG_HEALTH_DISTANCE) && defined(CONFIG_DEBUG)
	uart_set_r_cb(UART_BUS_ID(1), health_uart_r_int_handler);
#	endif
}


/* 多选 */
void comm_oneloop(void) {
	switch (comm_way) {
	default: 	ERROR("comm_way = %d\n", comm_way);
	break;case COMM_WAY_BLE:		ble_oneloop();
	break;case COMM_WAY_I2C:		i2c_oneloop();
	break;case COMM_WAY_SLE:		sle_oneloop();
	break;case COMM_WAY_WIFI:		wifi_oneloop();
	break;case COMM_WAY_UART:
		for (u8 id_oneloop=0; id_oneloop<3; id_oneloop++)
			if (uart_need_inited[id_oneloop])
				uart_oneloop(id_oneloop);
	}
}


/* 多选 */
void comm_exit(void) {
	ble_exit();
	i2c_exit();
	sle_exit();
	wifi_exit();

	for (u8 id_exit=0; id_exit<3; id_exit++)
		if (uart_need_inited[id_exit])
			uart_exit(id_exit);
}

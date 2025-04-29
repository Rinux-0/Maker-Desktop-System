#pragma once
#ifndef _COMM_DEF_H_
#define _COMM_DEF_H_



#if !defined(CONFIG_COMM_I2C)
#	define i2c_init()
#	define i2c_oneloop()
#	define i2c_exit()
#endif
#if !defined(CONFIG_COMM_UART)
#	define uart_init(id)
#	define uart_oneloop(id)
#	define uart_exit(id)
#	define uart_sle_r_int_handler		NULL
#	define uart_set_r_cb(id, cb)
#	define uart_write(id, data, length)
#endif
#if !defined(CONFIG_COMM_WIFI)
#	define wifi_init()
#	define wifi_oneloop()
#	define wifi_exit()
#	define wifi_uart_r_int_handler		NULL
// #	define wifi_set_r_cb(cb)
// #	define wifi_write(data, length)
#endif
#if !defined(CONFIG_COMM_BLE)
#	define ble_init()
#	define ble_oneloop()
#	define ble_exit()
#	define ble_uart_r_int_handler		NULL
// #	define ble_set_r_cb(cb)
// #	define ble_write(data, length)
#endif
#if !defined(CONFIG_COMM_SLE)
#	define sle_init()
#	define sle_oneloop()
#	define sle_exit()
#	define sle_uart_r_int_handler		NULL
#	define sle_set_r_cb(cb)
#	define sle_write(conn_id, data, length)
#endif



typedef enum {
	COMM_WAY_I2C,
	// COMM_WAY_USB,
	COMM_WAY_UART,
	COMM_WAY_WIFI,
	COMM_WAY_BLE,
	COMM_WAY_SLE,
	COMM_WAY_MAX,
	COMM_WAY_NONE
}comm_way_t;



#endif // _COMM_DEF_H_

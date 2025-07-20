#pragma once
#ifndef _COMM_DEF_H_
#define _COMM_DEF_H_



#if !defined(CONFIG_COMM_I2C)
#	define i2c_init()
#	define i2c_oneloop()
#	define i2c_exit()
#endif
#if !defined(CONFIG_COMM_SPI)
#	define spi_init(id)
#	define spi_oneloop(id)
#	define spi_exit(id)
#	define spi_write(id, data, length)
#endif
#if !defined(CONFIG_COMM_UART)
#	define uart_init(id, force_init)
#	define uart_oneloop(id)
#	define uart_exit(id)
#	define uart_set_r_cb(id, cb)
#	define uart_write(id, data, length)
#endif
#if !defined(CONFIG_COMM_SLE)
#	define sle_init()
#	define sle_oneloop()
#	define sle_exit()
#	define sle_set_r_cb(cb)
#	define sle_write(id, data, length)
#endif
#if !defined(CONFIG_COMM_WIFI)
#	define wifi__init()
#	define wifi_oneloop()
#	define wifi_exit()
#	define wifi_entry()
// #	define wifi_write(data, length)
#else
// #	error "receiver need to connect wifi first, please thinking twice before enable it"
#endif



// SPI: 0,1
#if defined(CONFIG_DEVICE_KEYBOARD)
#	define COMM_SPI_BUS_ID		(011)
#elif defined(CONFIG_DEVICE_KEYPAD) || defined(CONFIG_TEST_KEYTEST)
#	define COMM_SPI_BUS_ID		(010)
#elif !defined(CONFIG_COMM_SPI)
#	define COMM_SPI_BUS_ID		(000)
#endif



// UART: 0,1,2
#if defined(CONFIG_DEVICE_DYNAMIC) || defined(CONFIG_DEVICE_HEALTH) || defined(CONFIG_DEVICE_DESKAIDE)
#	define COMM_UART_BUS_ID		(0111)
#elif defined(CONFIG_DEVICE_KEYBOARD)
#	define COMM_UART_BUS_ID		(0011)
#elif ! defined(CONFIG_TEST_TMPTEST)
#	define COMM_UART_BUS_ID		(0010)
#else
#	define COMM_UART_BUS_ID		(0000)
#endif



typedef enum {
	COMM_WAY_I2C,
	COMM_WAY_SPI,
	COMM_WAY_UART,
	COMM_WAY_SLE,
	COMM_WAY_WIFI,

	COMM_WAY_MAX,
	COMM_WAY_NONE
}comm_way_t;



#endif // _COMM_DEF_H_

#pragma once
#ifndef _HEALTH_DEF_H_
#define _HEALTH_DEF_H_



#if !defined(CONFIG_DEVICE_HEALTH_DISTANCE)
#	define distance_init()
#	define distance_oneloop()
#	define distance_exit()
#endif
#if !defined(CONFIG_DEVICE_HEALTH_RATE)
#	define rate_uart_r_int_handler(buffer, length, error)
#	define rate_init()
#	define rate_oneloop()
#	define rate_exit()
#endif
#if !defined(CONFIG_DEVICE_HEALTH_TEMPERATURE)
#	define temperature_uart_r_int_handler(buffer, length, error)
#	define temperature_init()
#	define temperature_oneloop()
#	define temperature_exit()
#endif



#endif // _HEALTH_DEF_H_

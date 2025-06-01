#pragma once
#ifndef _HEALTH_DEF_H_
#define _HEALTH_DEF_H_



#if !defined(HEALTH_DISTANCE)
#	define distance_init()
#	define distance_oneloop()
#	define distance_exit()
#endif
#if !defined(HEALTH_FINGERPRINT)
#	define fingerprint_uart_r_int_handler(buffer, length, error)
#	define fingerprint_init()
#	define fingerprint_oneloop()
#	define fingerprint_exit()
#endif
#if !defined(HEALTH_HEART)
#	define heart_uart_r_int_handler(buffer, length, error)
#	define heart_init()
#	define heart_oneloop()
#	define heart_exit()
#endif
#if !defined(HEALTH_TEMPRETURE)
#	define tempreture_uart_r_int_handler(buffer, length, error)
#	define tempreture_init()
#	define tempreture_oneloop()
#	define tempreture_exit()
#endif



#endif // _HEALTH_DEF_H_

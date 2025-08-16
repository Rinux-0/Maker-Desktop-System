#ifndef _UUART_DEF_H_
#define _UUART_DEF_H_



#include "ddef.h"

#define UART_0_BUS_ID		(0)
#define UART_0_TXD_PIN		(17)
#define UART_0_RXD_PIN		(18)

#define UART_1_BUS_ID		(1)
#define UART_1_TXD_PIN		(15)
#define UART_1_RXD_PIN		(16)

#define UART_2_BUS_ID		(2)
#define UART_2_TXD_PIN		(8)
#define UART_2_RXD_PIN		(7)

#ifdef CONFIG_DEVICE_HEALTH_TEMPERATURE
#	define UART_BAUDRATE	(9600)
#else
#	define UART_BAUDRATE	(115200)
#endif

#define UART_DATA_BITS		(UART_DATA_BIT_8)
#define UART_STOP_BITS		(UART_STOP_BIT_1)
#define UART_PARITY_BIT		(UART_PARITY_NONE)
#define UART_TRANSFER_SIZE	(512)

typedef void (*uart_r_cb_t)(const void* buffer, u16 length, bool error);



#endif // _UUART_DEF_H_

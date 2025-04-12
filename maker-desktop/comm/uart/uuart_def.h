#ifndef _UUART_DEF_H_
#define _UUART_DEF_H_



#define UART_BUS_ID			(1)
#define UART_TXD_PIN		(15)
#define UART_RXD_PIN		(16)
#define UART_BAUDRATE		(9600) // (115200)
#define UART_DATA_BITS		(3)
#define UART_STOP_BITS		(1)
#define UART_PARITY_BIT		(0)
#define UART_TRANSFER_SIZE	(512)
#define UART_INT_TRANSFER_MODE	// UART_中断传输模式（此宏用于<uart.h>识别）



#endif // _UUART_DEF_H_

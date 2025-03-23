#ifndef _UART_CFG_DEF_H_
#define _UART_CFG_DEF_H_



// uart配置
#define UART_BUS_ID			1
#define UART_TXD_PIN		15
#define UART_RXD_PIN		16
#define UART_BAUDRATE		9600	// UART转HID模块 的要求
#define UART_DATA_BITS		3
#define UART_STOP_BITS		1
#define UART_PARITY_BIT		0
#define UART_TRANSFER_SIZE	512
#define UART_INT_TRANSFER_MODE	// UART_中断传输模式（此宏用于<uart.h>识别）

// debug配置 -保证 debug_uart用的bus_id 和 项目正常通讯用的bus_id 不同
#if defined(CONFIG_KEYPAD_DEBUG) && CONFIG_KEYPAD_DEBUG==1
#	ifdef UART_BUS_ID
#		define DEBUG_UART_BUS_ID !UART_BUS_ID
#	else
#		define DEBUG_UART_BUS_ID        0
#	endif
#   if DEBUG_UART_BUS_ID == 1
#       define DEBUG_UART_TXD_PIN		15
#       define DEBUG_UART_RXD_PIN		16
#   else
#       define DEBUG_UART_TXD_PIN		17
#       define DEBUG_UART_RXD_PIN		18
#   endif
#   define DEBUG_UART_BAUDRATE		115200
#endif // CONFIG_DEBUG



#endif /* _UART_CFG_DEF_H_ */

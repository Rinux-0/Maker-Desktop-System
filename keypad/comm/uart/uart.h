#ifndef _UART_H_
#define _UART_H_



void uart_init(void);
void uart_send_hid_pack(void);

#if defined(CONFIG_KEYPAD_DEBUG) && CONFIG_KEYPAD_DEBUG==1
#   include "../../util/util_def.h"
void debug_uart_init(void);
void debug_uart_print(const void* data, u32 length);
#else
#   define debug_uart_init()
#   define debug_uart_print(a, b)
#endif



#endif /* _UART_H_ */

#ifndef _UUART_H_
#define _UUART_H_



#include "def.h"
// #include "uart.h"
#include "uuart_def.h"
#include <errcode.h>

typedef void (*uart_rx_callback_t)(const void*, u16, bool);

errcode_t uart_set_rx_cb(uart_rx_callback_t uart_rx_cb);
void uart_init(void);
void uart_write(const u8* data, u32 len);



#endif // _UUART_H_

#ifndef _UUART_H_
#define _UUART_H_



#include "ddef.h"
#include "uuart_def.h"

extern bool uart_is_inited[3];

u8 UART_BUS_ID(u8 bus_id);

errcode_t uart_set_r_cb(u8 bus_id, uart_r_cb_t cb);
void uart_write(u8 bus_id, const u8* data, u32 length);

void uart_init(u8 bus_id, u32 baud_rate, bool force_init);
void uart_oneloop(u8 bus_id);
void uart_exit(u8 bus_id);



#endif // _UUART_H_

#ifndef _UUART_H_
#define _UUART_H_



#include "def.h"
// #include "uart.h"
#include "ssle_def.h"
#include "uuart_def.h"
#include <errcode.h>

typedef void (*uart_r_cb_t)(const void* buffer, u16 length, bool error);

errcode_t uart_set_r_cb(uart_r_cb_t cb);
void uart_init(void);
void uart_exit(void);
void uart_write(const u8* data, u32 length);
void uart_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status);



#endif // _UUART_H_

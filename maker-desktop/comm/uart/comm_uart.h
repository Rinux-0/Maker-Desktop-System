#ifndef _COMM_UART_H_
#define _COMM_UART_H_



#include "util_def.h"

void uart_init(void);
void uart_write(const u8* data, u32 len);



#endif // _COMM_UART_H_

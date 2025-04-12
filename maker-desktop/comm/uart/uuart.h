#ifndef _UUART_H_
#define _UUART_H_



#include "util_def.h"
#include "uuart_def.h"

void uart_init(void);
void uart_write(const u8* data, u32 len);



#endif // _UUART_H_

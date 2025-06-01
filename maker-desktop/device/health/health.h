#pragma once
#ifndef _HEALTH_H_
#define _HEALTH_H_



#include "health_def.h"

void health_uart_r_int_handler(const void* buffer, u16 length, bool error);

void health_init(void);
void health_oneloop(void);
void health_exit(void);



#endif // _HEALTH_H_

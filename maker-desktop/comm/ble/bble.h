#pragma once
#ifndef _BBLE_H_
#define _BBLE_H_



#include "bble_def.h"
#include "ddef.h"

void ble_uart_r_int_handler(const void* buffer, u16 length, bool error);

void ble_init(void);
void ble_oneloop(void);
void ble_exit(void);



#endif // _BBLE_H_

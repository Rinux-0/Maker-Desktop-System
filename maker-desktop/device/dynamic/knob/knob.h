#pragma once
#ifndef _KNOB_H_
#define _KNOB_H_



#include "ddef.h"

void knob_uart_r_int_handler(const void* buffer, u16 length, bool error);

void knob_init(void);
void knob_oneloop(void);
void knob_exit(void);



#endif // _KNOB_H_

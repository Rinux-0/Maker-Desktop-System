#pragma once
#ifndef _TEMPRETURE_H_
#define _TEMPRETURE_H_



#include "def.h"

void tempreture_get_head_req(void);
void tempreture_uart_r_int_handler(const void* buffer, u16 length, bool error);



#endif // _TEMPRETURE_H_

#pragma once
#ifndef _FINGERPRINT_H_
#define _FINGERPRINT_H_



#include "ddef.h"

void fingerprint_uart_r_int_handler(const void* buffer, u16 length, bool error);



#endif // _FINGERPRINT_H_

#pragma once
#ifndef _KP_CORE_H_
#define _KP_CORE_H_



#include "def.h"

void rcv_pin_init(void);
void rcv_past_update(void);
void rcv_now_read(void);
bool rcv_is_valid_diff(void);
void rcv_hid_wp_construct(void);
void rcv_hid_wp_send(void);



#endif // _KP_CORE_H_

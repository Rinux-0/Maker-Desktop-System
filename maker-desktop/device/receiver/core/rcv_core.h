#pragma once
#ifndef _KP_CORE_H_
#define _KP_CORE_H_



#include "ddef.h"

void rcv_init_pin(void);
void rcv_update_past(void);
void rcv_read_now(void);
bool rcv_is_valid_diff(void);
void rcv_set_hid_wp(void);
void rcv_send_hid_wp(void);



#endif // _KP_CORE_H_

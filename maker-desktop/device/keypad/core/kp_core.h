#pragma once
#ifndef _KP_CORE_H_
#define _KP_CORE_H_



#include "util_def.h"

void kp_pin_init(void);
void kp_past_update(void);
void kp_now_read(void);
bool kp_is_valid_diff(void);
void kp_hid_wp_construct(void);



#endif // _KP_CORE_H_

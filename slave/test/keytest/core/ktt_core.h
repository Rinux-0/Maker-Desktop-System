#pragma once
#ifndef _KTT_CORE_H_
#define _KTT_CORE_H_



#include "ddef.h"



void ktt_fn_processer(void);
void ktt_init_int_cb(void);
void ktt_init_pin(void);
bool ktt_is_fn_pressed(void);
bool ktt_is_valid_diff(void);
void ktt_read_now(void);
void ktt_send_hid_wp(void);
void ktt_set_ktt_hid_wp(void);
void ktt_update_past(void);



#endif // _KTT_CORE_H_

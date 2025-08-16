#pragma once
#ifndef _KBD_CORE_H_
#define _KBD_CORE_H_



#include "ddef.h"
#include "hhid_def.h"

void kbd_fn_processer(void);
void kbd_init_int_cb(void);
void kbd_init_pin(void);
bool kbd_is_fn_pressed(void);
bool kbd_is_valid_diff(void);
void kbd_read_now(void);
// bool kbd_merge_hid_wp(void);
void kbd_send_hid_wp(const hid_pack_t* wp);
void kbd_set_kbd_hid_wp(void);
void kbd_update_past(void);



#endif // _KBD_CORE_H_

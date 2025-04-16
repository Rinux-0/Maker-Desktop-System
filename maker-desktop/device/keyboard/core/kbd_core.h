#pragma once
#ifndef _KBD_CORE_H_
#define _KBD_CORE_H_



#include "def.h"

void kbd_pin_init(void);
void kbd_past_update(void);
void kbd_now_read(void);
bool kbd_is_valid_diff(void);
void kbd_hid_wp_construct(void);
void kbd_hid_wp_send(void);



#endif // _KBD_CORE_H_

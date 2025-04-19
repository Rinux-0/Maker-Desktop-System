#pragma once
#ifndef _KBD_CORE_H_
#define _KBD_CORE_H_



#include "def.h"

void kbd_set_hid_wp(void);
void kbd_init_pin(void);
bool kbd_is_valid_diff(void);
void kbd_read_now(void);
void kbd_send_hid_wp(void);
void kbd_update_past(void);



#endif // _KBD_CORE_H_

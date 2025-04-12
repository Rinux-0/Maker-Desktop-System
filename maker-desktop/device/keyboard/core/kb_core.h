#pragma once
#ifndef _KB_CORE_H_
#define _KB_CORE_H_



#include "util_def.h"

void kb_pin_init(void);
void kb_past_update(void);
void kb_now_read(void);
bool kb_is_valid_diff(void);
void kb_hid_wp_construct(void);



#endif // _KB_CORE_H_

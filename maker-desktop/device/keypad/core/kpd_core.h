#pragma once
#ifndef _KPD_CORE_H_
#define _KPD_CORE_H_



#include "def.h"

void kpd_pin_init(void);
void kpd_past_update(void);
void kpd_now_read(void);
bool kpd_is_valid_diff(void);
void kpd_hid_wp_construct(void);
void kpd_hid_wp_send(void);



#endif // _KPD_CORE_H_

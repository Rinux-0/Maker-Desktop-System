#pragma once
#ifndef _KPD_CORE_H_
#define _KPD_CORE_H_



#include "ddef.h"

void kpd_init_pin(void);
void kpd_update_past(void);
void kpd_read_now(void);
bool kpd_is_valid_diff(void);
void kpd_set_hid_wp(void);
void kpd_send_hid_wp(void);



#endif // _KPD_CORE_H_

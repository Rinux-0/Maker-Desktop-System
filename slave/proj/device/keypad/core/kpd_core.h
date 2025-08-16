#pragma once
#ifndef _KPD_CORE_H_
#define _KPD_CORE_H_



#include "ddef.h"
#include "hhid_def.h"



void kpd_fn_processer(void);
void kpd_init_int_cb(void);
void kpd_init_pin(void);
bool kpd_is_fn_pressed(void);
bool kpd_is_valid_diff(void);
void kpd_read_now(void);
// bool kpd_merge_hid_wp(void);
void kpd_send_hid_wp(const hid_pack_t* wp);
void kpd_set_kpd_hid_wp(void);
void kpd_update_past(void);



#endif // _KPD_CORE_H_

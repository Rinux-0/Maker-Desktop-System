#pragma once
#ifndef _VFD_H_
#define _VFD_H_



#include "ddef.h"
#include "ssle.h"
#include "vfd_def.h"

vfd_mode_t vfd_get_mode(void);
void vfd_set_mode(vfd_mode_t mode);
void vfd_set_mode_next(void);

void vfd_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status);

// void vfd_init(void);
// void vfd_oneloop(void);
// void vfd_exit(void);
void vfd_entry(void);



#endif // _VFD_H_

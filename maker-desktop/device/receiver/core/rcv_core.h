#pragma once
#ifndef _RCV_CORE_H_
#define _RCV_CORE_H_



#include "ssle.h"

void rcv_init_pin(void);
void rcv_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status);



#endif // _RCV_CORE_H_

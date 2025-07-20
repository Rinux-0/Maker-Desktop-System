#pragma once
#ifndef _VFD_MODE_H_
#define _VFD_MODE_H_



#include "ddef.h"

void vfd_update_sle_conn_status(u8 index, bool status);

void vfd_show_mode_charge(void);
void vfd_show_mode_conn(void);



#endif // _VFD_MODE_H_

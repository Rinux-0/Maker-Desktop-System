#pragma once
#ifndef VFD_CORE_H_
#define VFD_CORE_H_



#include "ddef.h"

void vfd_core_set_pos(u8 x, u8 y);

void vfd_core_flush_line(u8 line);
void vfd_core_flush_screen(void);

void vfd_core_set_char(c8 ch, u8 x, u8 y, bool flush);
void vfd_core_set_line(u8* buff_20, u8 line, bool flush);
void vfd_core_set_screen(u8* l1_20, u8* l2_20, bool flush);

// void vfd_core_clear_line(u8 line);
void vfd_core_clear_line_back(u8 line);
void vfd_core_clear_screen(void);

void vfd_cmd_entry(c8* buff, u16 length);



#endif // VFD_CORE_H_

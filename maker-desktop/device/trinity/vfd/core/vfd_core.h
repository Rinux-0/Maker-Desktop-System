#pragma once
#ifndef VFD_CORE_H_
#define VFD_CORE_H_



void vfd_core_flush_line(u8 line);
void vfd_core_flush_screen(void);
void vfd_core_set_char(c8 ch, u8 x, u8 y, bool flush);
void vfd_core_set_line(u8* buff_20, u8 line, bool flush);
void vfd_core_set_screen(u8* l1_20, u8* l2_20, bool flush);



#endif // VFD_CORE_H_

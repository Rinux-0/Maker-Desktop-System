#pragma once
#ifndef _COLOR_H_
#define _COLOR_H_



#include "ddef.h"
#include "color_def.h"

color_mode_t color_get_mode(void);
void color_set_mode(color_mode_t mode);
void color_set_mode_next(void);

void color_ctrl_hsv_h(s8 speed);
void color_ctrl_hsv_v(bool is_changing, float v_max, u8 which_is_valid);
void color_ctrl_hsv(color_hsv_t color_hsv);

void color_show_0(u16 num, double dev_spd);
void color_show_1(u16 num, u8* spi_buff);

void color_init(void);
void color_oneloop(void);
void color_exit(void);



#endif // _COLOR_H_

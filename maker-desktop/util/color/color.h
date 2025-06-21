#pragma once
#ifndef _COLOR_H_
#define _COLOR_H_



#include "ddef.h"
#include "color_def.h"

void color_ctrl_hsv_h(s8 speed);
void color_ctrl_hsv_s(bool is_changing, bool is_full, u8 which_is_valid);
void color_ctrl_hsv_v(bool is_changing, bool is_full, u8 which_is_valid);

color_mode_t color_get_mode(void);
void color_set_mode(color_mode_t mode);
void color_set_mode_next(void);
void color_show(u16 num, double spd_rgb);



#endif // _COLOR_H_

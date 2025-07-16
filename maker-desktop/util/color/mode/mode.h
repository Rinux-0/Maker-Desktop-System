#pragma once
#ifndef _COLOR_MODE_H_
#define _COLOR_MODE_H_



#include "ddef.h"
#include "../color_def.h"

extern color_mode_t color_mode;

void color_show_mode_breath(u16 num, double dev_spd);
void color_show_mode_flow(u16 num, double dev_spd);
void color_show_mode_mess(u16 num, double dev_spd);
void color_show_mode_static(u16 num, double dev_spd);



#endif // _COLOR_MODE_H_

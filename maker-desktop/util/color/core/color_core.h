#pragma once
#ifndef _COLOR_CORE_H_
#define _COLOR_CORE_H_



#include "ddef.h"
#include "../color_def.h"

extern color_rgb_t rgb[2];
extern color_hsv_t hsv[2];

extern s8 color_h_speed;
extern bool color_s_is_full;
extern bool color_v_is_full;
extern bool color_s_is_changing;
extern bool color_v_is_changing;

void color_core_update_sv(void);
void color_core_hsv2rgb(void);

void color_core_set_spi_data(u8 spi_bus_id);

void color_core_show(u8 spi_bus_id);
void color_core_reset(u8 spi_bus_id, u8 key_num);


#endif // _COLOR_CORE_H_

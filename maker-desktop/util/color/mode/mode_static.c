#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"
#include "../core/color_core.h"



void color_show_mode_static(u16 num, double dev_spd) {
	if (color_h_speed != -3) {
		hsv[0].h = 60 * color_h_speed + 120;	// 用作 60度色相
		color_s_is_full = true;
	} else {
		color_s_is_changing = false;
		color_s_is_full = false;
	}

	color_core_update_sv();
	color_core_hsv2rgb();
	color_core_set_spi_data(0);

	for (u16 i = 0; i < num; i++)
		color_core_show(0);
}

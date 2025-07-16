#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"
#include "../core/color_core.h"



void color_show_mode_static(u16 num, double dev_spd) {
	if (color_h_speed != 5) {
		hsv.h = 36 * color_h_speed + 180;
		hsv.s = 1.f;
	} else {
		hsv.s = 0.f;
	}

	color_core_update_v();
	color_core_hsv2rgb();

	for (u16 i = 0; i < num; i++)
		color_core_set_spi_data(i);

	color_core_show(0, num, NULL);
}

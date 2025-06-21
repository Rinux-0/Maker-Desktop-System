#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"
#include "../core/color_core.h"



static u16 time;



void color_show_mode_mess(u16 num, double dev_spd) {
	color_core_update_sv();

	if (time++ < 1024 * 1.5 / abs(color_h_speed))
		return;
	time = 0;

	for (u16 i = 0; i < num; i++) {
		hsv.h = 360.f * rand() / RAND_MAX;

		color_core_hsv2rgb();
		color_core_set_spi_data();
		color_core_show();
	}
}

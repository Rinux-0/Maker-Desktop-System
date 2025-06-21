#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"
#include "../core/color_core.h"



void color_show_mode_breath(u16 num, double dev_spd) {
	hsv.h = .25f * color_h_speed * dev_spd + hsv.h;
	if (hsv.h >= 360.f) {
		hsv.h -= 360.f;
	} else if (hsv.h < 0.f) {
		hsv.h += 360.f;
	}

	color_core_update_sv();
	color_core_hsv2rgb();
	color_core_set_spi_data();

	// DATA("[ HSV.h: %d.%d%d ]\n",
	// 	(u16)hsv.h, (u8)(hsv.h * 10) % 10, (u8)(hsv.h * 100) % 10
	// );

	for (u16 i = 0; i < num; i++)
		color_core_show();
}

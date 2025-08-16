#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"
#include "../core/color_core.h"



void color_show_mode_flow(u16 num, double dev_spd) {
	float hsv_h = .25f * color_h_speed * dev_spd + hsv.h;
	if (hsv_h >= 360.f) {
		hsv_h -= 360.f;
	} else if (hsv_h < 0.f) {
		hsv_h += 360.f;
	}

	// DATA("\n\t[ HSV.h: %d.%d%d ]\n",
	// 	(u16)hsv_h, (u8)(hsv_h * 10) % 10, (u8)(hsv_h * 100) % 10
	// );

	color_core_update_v();

	for (u16 i = 0; i < num; i++) {
		hsv.h = hsv_h + 120.f * i / num;
		if (hsv.h >= 360.f) {
			hsv.h -= 360.f;
		} else if (hsv.h < 0.f) {
			hsv.h += 360.f;
		}

		color_core_hsv2rgb();

		color_core_set_spi_data(i);
	}

	color_core_show(0, num, NULL);

	hsv.h = hsv_h;
}

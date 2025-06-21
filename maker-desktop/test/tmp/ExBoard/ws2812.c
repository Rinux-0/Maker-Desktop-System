#include "ws2812.h"

#include "ddef.h"
#include "ttool.h"
#include "sspi.h"



static color_hsv_t hsv = {
	.h = 0.f,
	.s = 1.f,
	.v = 1.f
};


void ws2812_init(void) {
	spi_init();
}


void ws2812_oneloop(void) {
	hsv.h = (u16)(hsv.h + .1f) / 360
		? .0f
		: .25f + hsv.h;

	// DATA("[ HSV.h: %d.%d%d ]\n",
	// 	(u16)hsv.h, (u8)(hsv.h * 10) % 10, (u8)(hsv.h * 100) % 10
	// );

	for (u8 i = 0; i < 16; i++) {
		tool_color_spi_write_hsv(&hsv);
	}

	LOG("");

	spi_write(NULL, 0);
	// tool_delay_u(5000);
}


void ws2812_exit(void) {
	spi_exit();
}

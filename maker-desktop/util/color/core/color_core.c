#include "color_core.h"

#include "ddef.h"
#include "ttool.h"

#include "../color_def.h"
#include "sspi.h"



color_hsv_t hsv = {
	.h = 0.f,
	.s = 1.f,
	.v = .5f
};
color_rgb_t rgb;
static u8 spi_data[NUM_KEY][24];

s8 color_h_speed = 1;
float color_v_max = 1.f;
bool color_v_is_changing;



void color_core_update_v(void) {
	static s8 v_chage_dir = 1;		// 1: up, -1: down

	if (color_v_is_changing) {
		hsv.v += color_v_max * v_chage_dir * .0015f;
	} else {
		hsv.v = color_v_max;
	}

	if (hsv.v > color_v_max) {
		hsv.v = color_v_max;
		if (color_v_is_changing)
			v_chage_dir = -1;
	} else if (hsv.v < 0.f) {
		hsv.v = 0.f;
		if (color_v_is_changing)
			v_chage_dir = 1;
	}
}


void color_core_hsv2rgb(void) {
	/* HSV */
	float c = hsv.s/* * hsv.v*/; 	// 色度
	float x = c * (1 - fabsf(fmodf(hsv.h / 60, 2) - 1));
	float m = 1/*hsv.v*/ - c;

	float r_prime, g_prime, b_prime;

	if (hsv.h >= 0 && hsv.h < 60) {
		r_prime = c; g_prime = x; b_prime = 0;
	} else if (hsv.h < 120) {
		r_prime = x; g_prime = c; b_prime = 0;
	} else if (hsv.h < 180) {
		r_prime = 0; g_prime = c; b_prime = x;
	} else if (hsv.h < 240) {
		r_prime = 0; g_prime = x; b_prime = c;
	} else if (hsv.h < 300) {
		r_prime = x; g_prime = 0; b_prime = c;
	} else {
		r_prime = c; g_prime = 0; b_prime = x;
	}

	rgb.r = (u8)((r_prime + m) * 255 * hsv.v);
	rgb.g = (u8)((g_prime + m) * 255 * hsv.v);
	rgb.b = (u8)((b_prime + m) * 255 * hsv.v);

	/* DBG */
	// DATA("[HSV: %d.%d%d, %d.%d%d, %d.%d%d] -> [cxm: %d.%d%d, %d.%d%d, %d.%d%d] -> [RGB: %d, %d, %d]\n",
	// 	(u16)hsv.h, (u8)(hsv.h * 10) % 10, (u8)(hsv.h * 100) % 10,
	// 	(u16)hsv.s, (u8)(hsv.s * 10) % 10, (u8)(hsv.s * 100) % 10,
	// 	(u16)hsv.v, (u8)(hsv.v * 10) % 10, (u8)(hsv.v * 100) % 10,
	// 	(u16)c, (u8)(c * 10) % 10, (u8)(c * 100) % 10,
	// 	(u16)x, (u8)(x * 10) % 10, (u8)(x * 100) % 10,
	// 	(u16)m, (u8)(m * 10) % 10, (u8)(m * 100) % 10,
	// 	rgb.r, rgb.g, rgb.b
	// );
	// DATA("\n\t[HSV: %d.%d%d, %d.%d%d, %d.%d%d]\n",
	// 	(u16)hsv.h, (u8)(hsv.h * 10) % 10, (u8)(hsv.h * 100) % 10,
	// 	(u16)hsv.s, (u8)(hsv.s * 10) % 10, (u8)(hsv.s * 100) % 10,
	// 	(u16)hsv.v, (u8)(hsv.v * 10) % 10, (u8)(hsv.v * 100) % 10
	// );
	// DATA("\n\t[RGB: %d, %d, %d]\n",
	// 	rgb.r, rgb.g, rgb.b
	// );
}


void color_core_set_spi_data(u8 key_num_id) {
	// GRB（高位在前）
	for (u8 j = 0; j < 8; j++) {
		spi_data[key_num_id][j + 0 * 8] = (rgb.g & (1 << (7 - j))) ? SPI_TRUE : SPI_FALSE;
		spi_data[key_num_id][j + 1 * 8] = (rgb.r & (1 << (7 - j))) ? SPI_TRUE : SPI_FALSE;
		spi_data[key_num_id][j + 2 * 8] = (rgb.b & (1 << (7 - j))) ? SPI_TRUE : SPI_FALSE;
	}

	// DATA("[RGB: %d, %d, %d]\n",
	// 	rgb.r, rgb.g, rgb.b
	// );
}


void color_core_show(u8 spi_bus_id, u8 key_num, u8* spi_buff) {
	switch (spi_bus_id) {
	default:
	break;case 0:	spi_write(spi_bus_id, *spi_data, 24 * key_num);
	break;case 1:	spi_write(spi_bus_id, spi_buff, 24 * key_num);
	}
}


void color_core_reset(u8 spi_bus_id, u8 key_num) {
	spi_write_empty(spi_bus_id, key_num);
}

#include "color_core.h"

#include "ddef.h"
#include "ttool.h"

#include "../color_def.h"
#include "sspi.h"



static u16 time;

static u8 spi_data[24];

color_rgb_t rgb;
color_hsv_t hsv = {
	.h = 0.f,
	.s = 1.f,
	.v = 1.f
};

s8 color_h_speed = 1;
bool color_s_is_full = true;
bool color_v_is_full = true;
bool color_s_is_changing;
bool color_v_is_changing;



void color_core_update_sv(void) {
	static s8 s_up_or_dn = 1;
	static s8 v_up_or_dn = 1;

	if (time++ < 1024 * 1.5)
		return;

	time = 0;

	if (color_s_is_changing) {
		hsv.s += 0.01f * s_up_or_dn;
		if (hsv.s >= 1.f || hsv.s <= 0.f) {
			hsv.s = s_up_or_dn == 1;
			s_up_or_dn = -s_up_or_dn;
		}
	} else {
		hsv.s = color_s_is_full;
	}

	if (color_v_is_changing) {
		hsv.v += 0.01f * v_up_or_dn;
		if (hsv.v >= 1.f || hsv.v <= 0.f) {
			hsv.v = v_up_or_dn == 1;
			v_up_or_dn = -v_up_or_dn;
		}
	} else {
		hsv.v = color_v_is_full;
	}
}


void color_core_hsv2rgb(void) {
	/* HSV */
	float c = hsv.s/* * hsv.s*/; // 色度
	float x = c * (1 - fabs(fmod(hsv.h / 60.0f, 2) - 1));
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
}


void color_core_set_spi_data(void) {
	// GRB（高位在前）
	for (u8 i = 0; i < 8; i++) {
		spi_data[i + 0 * 8] = (rgb.g & (1 << (7 - i))) ? SPI_TRUE : SPI_FALSE;
		spi_data[i + 1 * 8] = (rgb.r & (1 << (7 - i))) ? SPI_TRUE : SPI_FALSE;
		spi_data[i + 2 * 8] = (rgb.b & (1 << (7 - i))) ? SPI_TRUE : SPI_FALSE;
	}

	// DATA("[RGB: %d, %d, %d]\n",
	// 	rgb.r, rgb.g, rgb.b
	// );
}


void color_core_show(void) {
	spi_write(spi_data, 24);
}

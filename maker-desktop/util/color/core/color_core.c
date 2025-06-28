#include "color_core.h"

#include "ddef.h"
#include "ttool.h"

#include "../color_def.h"
#include "sspi.h"



static u16 time;

static u8 spi_data[2][24];

color_rgb_t rgb[2];
color_hsv_t hsv[2] = {
	{
		.h = 0.f,
		.s = 1.f,
		.v = 1.f
	}
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
		hsv[0].s += 0.01f * s_up_or_dn;
		if (hsv[0].s >= 1.f || hsv[0].s <= 0.f) {
			hsv[0].s = s_up_or_dn == 1;
			s_up_or_dn = -s_up_or_dn;
		}
	} else {
		hsv[0].s = color_s_is_full;
	}

	if (color_v_is_changing) {
		hsv[0].v += 0.01f * v_up_or_dn;
		if (hsv[0].v >= 1.f || hsv[0].v <= 0.f) {
			hsv[0].v = v_up_or_dn == 1;
			v_up_or_dn = -v_up_or_dn;
		}
	} else {
		hsv[0].v = color_v_is_full;
	}
}


void color_core_hsv2rgb(void) {
	/* HSV */
	float c = hsv[0].s/* * hsv[0].s*/; // 色度
	float x = c * (1 - fabs(fmod(hsv[0].h / 60.0f, 2) - 1));
	float m = 1/*hsv[0].v*/ - c;

	float r_prime, g_prime, b_prime;

	if (hsv[0].h >= 0 && hsv[0].h < 60) {
		r_prime = c; g_prime = x; b_prime = 0;
	} else if (hsv[0].h < 120) {
		r_prime = x; g_prime = c; b_prime = 0;
	} else if (hsv[0].h < 180) {
		r_prime = 0; g_prime = c; b_prime = x;
	} else if (hsv[0].h < 240) {
		r_prime = 0; g_prime = x; b_prime = c;
	} else if (hsv[0].h < 300) {
		r_prime = x; g_prime = 0; b_prime = c;
	} else {
		r_prime = c; g_prime = 0; b_prime = x;
	}

	rgb[0].r = (u8)((r_prime + m) * 255 * hsv[0].v);
	rgb[0].g = (u8)((g_prime + m) * 255 * hsv[0].v);
	rgb[0].b = (u8)((b_prime + m) * 255 * hsv[0].v);

	/* DBG */
	// DATA("[HSV: %d.%d%d, %d.%d%d, %d.%d%d] -> [cxm: %d.%d%d, %d.%d%d, %d.%d%d] -> [RGB: %d, %d, %d]\n",
	// 	(u16)hsv[0].h, (u8)(hsv[0].h * 10) % 10, (u8)(hsv[0].h * 100) % 10,
	// 	(u16)hsv[0].s, (u8)(hsv[0].s * 10) % 10, (u8)(hsv[0].s * 100) % 10,
	// 	(u16)hsv[0].v, (u8)(hsv[0].v * 10) % 10, (u8)(hsv[0].v * 100) % 10,
	// 	(u16)c, (u8)(c * 10) % 10, (u8)(c * 100) % 10,
	// 	(u16)x, (u8)(x * 10) % 10, (u8)(x * 100) % 10,
	// 	(u16)m, (u8)(m * 10) % 10, (u8)(m * 100) % 10,
	// 	rgb[0].r, rgb[0].g, rgb[0].b
	// );
}


void color_core_set_spi_data(u8 spi_bus_id) {
	// GRB（高位在前）
	for (u8 i = 0; i < 8; i++) {
		spi_data[0][i + 0 * 8] = (rgb[spi_bus_id].g & (1 << (7 - i))) ? SPI_TRUE : SPI_FALSE;
		spi_data[0][i + 1 * 8] = (rgb[spi_bus_id].r & (1 << (7 - i))) ? SPI_TRUE : SPI_FALSE;
		spi_data[0][i + 2 * 8] = (rgb[spi_bus_id].b & (1 << (7 - i))) ? SPI_TRUE : SPI_FALSE;
	}

	// DATA("[RGB: %d, %d, %d]\n",
	// 	rgb[spi_bus_id].r, rgb[spi_bus_id].g, rgb[spi_bus_id].b
	// );
}


void color_core_show(u8 spi_bus_id) {
	spi_write(spi_bus_id, spi_data[spi_bus_id], 24);
}


void color_core_reset(u8 spi_bus_id) {
	spi_write_empty(spi_bus_id);
}

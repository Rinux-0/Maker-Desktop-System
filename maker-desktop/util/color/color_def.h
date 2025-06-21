#pragma once
#ifndef _COLOR_DEF_H_
#define _COLOR_DEF_H_



typedef struct {
	u8 r, g, b;
} color_rgb_t;

typedef struct {
	float h; // 色相	[ 0.0, 360.0 )
	float s; // 饱和度	[ 0.0, 1.0 ]
	float v; // 明度	[ 0.0, 1.0 ]
} color_hsv_t;

typedef enum {
	COLOR_MODE_OFF,
	COLOR_MODE_BREATH,
	COLOR_MODE_FLOW,
	COLOR_MODE_MESS,
	COLOR_MODE_STATIC,
	COLOR_MODE_MAX
}color_mode_t;



#endif // _COLOR_DEF_H_

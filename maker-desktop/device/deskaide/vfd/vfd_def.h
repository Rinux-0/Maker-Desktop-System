#pragma once
#ifndef _VFD_DEF_H_
#define _VFD_DEF_H_



#define VFD_GPIO_INT_PIN 11

typedef enum {
	VFD_MODE_OFF,
	VFD_MODE_CHARGE,
	VFD_MODE_CONN,		// sle bt_snd
	VFD_MODE_MAX
} vfd_mode_t;



#endif // _VFD_DEF_H_

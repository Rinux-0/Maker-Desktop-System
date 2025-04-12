#pragma once
#ifndef _DEVICE_H_
#define _DEVICE_H_



#include "util_def.h"

typedef enum {
	DEVICE_KEYBOARD, DEVICE_KEYPAD, DEVICE_MOUSE,
	DEVICE_POWER, DEVICE_SOUND, DEVICE_LAMP, DEVICE_RECEIVER,
	DEVICE_MAX, DEVICE_NONE = -1
}device_sle_index;

void device_init(void);
void device_oneloop(void);
void device_exit(void);



#endif // _DEVICE_H_

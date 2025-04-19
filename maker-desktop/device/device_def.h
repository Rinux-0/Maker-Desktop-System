#pragma once
#ifndef _DEVICE_DEF_H_
#define _DEVICE_DEF_H_



#if defined(CONFIG_DEVICE_RECEIVER)
#define DEVICE_NAME receiver
#elif defined(CONFIG_DEVICE_KEYBOARD)
#define DEVICE_NAME keyboard
#elif defined(CONFIG_DEVICE_KEYPAD)
#define DEVICE_NAME keypad
#elif defined(CONFIG_DEVICE_LAMP)
#define DEVICE_NAME lamp
#elif defined(CONFIG_DEVICE_MOUSE)
#define DEVICE_NAME mouse
#elif defined(CONFIG_DEVICE_UNION)
#define DEVICE_NAME union
#else
#	error "FATAL_ERROR This device can't be recognized!"
#endif



#include "def.h"

#define HEADER(path, name_main, name_expand) STR(path\\name_main.name_expand)

#define INIT(obj) CONCAT2(obj, _init)
#define ONELOOP(obj) CONCAT2(obj, _oneloop)
#define EXIT(obj) CONCAT2(obj, _exit)



#endif // _DEVICE_DEF_H_

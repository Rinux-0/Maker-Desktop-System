#pragma once
#ifndef _DEVICE_DEF_H_
#define _DEVICE_DEF_H_



#if defined(CONFIG_DEVICE_DESKAIDE)
#	define DEVICE_NAME deskaide
#elif defined(CONFIG_DEVICE_DYNAMIC)
#	define DEVICE_NAME dynamic
#elif defined(CONFIG_DEVICE_HEALTH)
#	define DEVICE_NAME health
#elif defined(CONFIG_DEVICE_KEYBOARD)
#	define DEVICE_NAME keyboard
#elif defined(CONFIG_DEVICE_KEYPAD)
#	define DEVICE_NAME keypad
#elif defined(CONFIG_DEVICE_LAMP)
#	define DEVICE_NAME lamp
#elif defined(CONFIG_DEVICE_MOUSE)
#	define DEVICE_NAME mouse
#elif defined(CONFIG_DEVICE_RECEIVER)
#	define DEVICE_NAME receiver
#endif



#endif // _DEVICE_DEF_H_

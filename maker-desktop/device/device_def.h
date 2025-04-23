#pragma once
#ifndef _DEVICE_DEF_H_
#define _DEVICE_DEF_H_



#if defined(CONFIG_DEVICE_RECEIVER)
#	define DEVICE_NAME receiver
#elif defined(CONFIG_DEVICE_KEYBOARD)
#	define DEVICE_NAME keyboard
#elif defined(CONFIG_DEVICE_KEYPAD)
#	define DEVICE_NAME keypad
#elif defined(CONFIG_DEVICE_KNOB)
#	define DEVICE_NAME knob
#elif defined(CONFIG_DEVICE_LAMP)
#	define DEVICE_NAME lamp
#elif defined(CONFIG_DEVICE_MOUSE)
#	define DEVICE_NAME mouse
#elif defined(CONFIG_DEVICE_NFC)
#	define DEVICE_NAME nfc
#elif defined(CONFIG_DEVICE_UNION)
#	define DEVICE_NAME union
#else
#	error FATAL_ERROR ("This device can't be recognized!")
#endif



#endif // _DEVICE_DEF_H_

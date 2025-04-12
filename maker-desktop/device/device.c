#include "device.h"

#if defined(CONFIG_DEVICE_KEYBOARD)
#	include "keyboard/keyboard.h"
#elif defined(CONFIG_DEVICE_KEYPAD)
#	include "keypad/keypad.h"
#elif defined(CONFIG_DEVICE_LAMP)
#	include "lamp/lamp.h"
#elif defined(CONFIG_DEVICE_MOUSE)
#	include "mouse/mouse.h"
#elif defined(CONFIG_DEVICE_RECEIVER)
#	include "receiver/receiver.h"
#elif defined(CONFIG_DEVICE_UNION)
#	include "union/union.h"
#else
#	error "This device can't be recognized."
#endif



// static const u8 device_name[] =
// #if defined(CONFIG_DEVICE_KEYBOARD)
// "kbd"
// #elif defined(CONFIG_DEVICE_KEYPAD)
// "kpd"
// #elif defined(CONFIG_DEVICE_LAMP)
// "lmp"
// #elif defined(CONFIG_DEVICE_MOUSE)
// "mus"
// #elif defined(CONFIG_DEVICE_UNION)
// "uni"
// #elif defined(CONFIG_DEVICE_RECEIVER)
// "rcv"
// #endif
// ;



void device_init(void) {
	#if defined(CONFIG_DEVICE_KEYBOARD)
	keyboard_init();
	#elif defined(CONFIG_DEVICE_KEYPAD)
	keypad_init();
	#elif defined(CONFIG_DEVICE_MOUSE)
	mouse_init();
	#elif defined(CONFIG_DEVICE_POWER)
	power_init();
	#elif defined(CONFIG_DEVICE_SOUND)
	sound_init();
	#elif defined(CONFIG_DEVICE_LAMP)
	lamp_init();
	#elif defined(CONFIG_DEVICE_RECEIVER)
	receiver_init();
	#endif

	#if defined(CONFIG_ENABLE_DEVICE_MANAGER)
	device_sle_map_ask();
	#endif
}


void device_oneloop(void) {
	#if defined(CONFIG_DEVICE_KEYBOARD)
	keyboard_oneloop();
	#elif defined(CONFIG_DEVICE_KEYPAD)
	keypad_oneloop();
	#elif defined(CONFIG_DEVICE_MOUSE)
	mouse_oneloop();
	#elif defined(CONFIG_DEVICE_POWER)
	power_oneloop();
	#elif defined(CONFIG_DEVICE_SOUND)
	sound_oneloop();
	#elif defined(CONFIG_DEVICE_LAMP)
	lamp_oneloop();
	#elif defined(CONFIG_DEVICE_RECEIVER)
	receiver_oneloop();
	#endif
}


void device_exit(void) {
	#if defined(CONFIG_DEVICE_KEYBOARD)
	keyboard_exit();
	#elif defined(CONFIG_DEVICE_KEYPAD)
	keypad_exit();
	#elif defined(CONFIG_DEVICE_MOUSE)
	mouse_exit();
	#elif defined(CONFIG_DEVICE_POWER)
	power_exit();
	#elif defined(CONFIG_DEVICE_SOUND)
	sound_exit();
	#elif defined(CONFIG_DEVICE_LAMP)
	lamp_exit();
	#elif defined(CONFIG_DEVICE_RECEIVER)
	receiver_exit();
	#endif
}

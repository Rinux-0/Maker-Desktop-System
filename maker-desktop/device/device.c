#include "device.h"

#include "comm_uart.h"
#include "util_tool.h"
#if defined(CONFIG_DEVICE_KEYBOARD)
#	include "keyboard/keyboard.h"
#elif defined(CONFIG_DEVICE_KEYPAD)
#	include "keypad/keypad.h"
#elif defined(CONFIG_DEVICE_MOUSE)
#	include "mouse/mouse.h"
#elif defined(CONFIG_DEVICE_POWER)
#	include "power/power.h"
#elif defined(CONFIG_DEVICE_SOUND)
#	include "sound/sound.h"
#elif defined(CONFIG_DEVICE_LAMP)
#	include "lamp/lamp.h"
#elif defined(CONFIG_DEVICE_RECEIVER)
#	include "receiver/receiver.h"
#endif



static const u8 device_name[] =
#if defined(CONFIG_DEVICE_KEYBOARD)
"keyboard";
#elif defined(CONFIG_DEVICE_KEYPAD)
"keypad";
#elif defined(CONFIG_DEVICE_MOUSE)
"mouse";
#elif defined(CONFIG_DEVICE_POWER)
"power";
#elif defined(CONFIG_DEVICE_SOUND)
"sound";
#elif defined(CONFIG_DEVICE_LAMP)
"lamp";
#elif defined(CONFIG_DEVICE_RECEIVER)
"receiver";
#else
"none";
#endif



#ifdef CONFIG_ENABLE_DEVICE_MANAGER
static u8 device_identify_id;
static u8 device_sle_map[DEVICE_MAX] = {};



u8 device_get_sle_id(device_sle_index device) {
	return device_sle_map[device];
}


static void device_sle_map_ask(void) {
	u8 question[] = " #What are you?\r\n";
	for (device_identify_id=0; device_identify_id<DEVICE_MAX; device_identify_id++) {
		question[0] = device_identify_id + '0';
		uart_write(question, sizeof question);
		mdelay(250);
	}
}


void device_sle_map_fill(const u8* pos) {
	if (strcmp(pos, "keyboard") == 0) {
		device_sle_map[DEVICE_KEYBOARD] = device_identify_id;
	} else if (strcmp(pos, "keypad") == 0) {
		device_sle_map[DEVICE_KEYPAD] = device_identify_id;
	} else if (strcmp(pos, "mouse") == 0) {
		device_sle_map[DEVICE_MOUSE] = device_identify_id;
	} else if (strcmp(pos, "power") == 0) {
		device_sle_map[DEVICE_POWER] = device_identify_id;
	} else if (strcmp(pos, "sound") == 0) {
		device_sle_map[DEVICE_SOUND] = device_identify_id;
	} else if (strcmp(pos, "lamp") == 0) {
		device_sle_map[DEVICE_LAMP] = device_identify_id;
	} else if (strcmp(pos, "receiver") == 0) {
		device_sle_map[DEVICE_RECEIVER] = device_identify_id;
	}
}


#else
void device_sle_map_reply(const u8* pos) {
	static u8 reply[32];

	strcpy(reply, device_name);

	if (strcmp(pos, "What are you?") == 0) {
		reply[0] = '#';
		uart_write(reply, sizeof device_name + 1);
	}
}


#endif


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

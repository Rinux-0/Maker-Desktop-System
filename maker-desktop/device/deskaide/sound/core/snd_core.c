#include "snd_core.h"

#include "ddef.h"
#include "ttool.h"



#define KEY_K1 14
#define KEY_K2 13
#define KEY_K3 5

static volatile bool is_wating;



static void snd_core_click_twice(pin_t pin) {
	tool_pin_gpio_refresh_m(pin, 200);
	tool_sleep_m(200);
	tool_pin_gpio_refresh_m(pin, 200);
}


void snd_core_init_pin(void) {
	// GPIO_0
	uapi_pin_set_mode(KEY_K1, PIN_MODE_0);
	uapi_gpio_set_dir(KEY_K1, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KEY_K1, GPIO_LEVEL_HIGH);

	// GPIO_1
	uapi_pin_set_mode(KEY_K2, PIN_MODE_0);
	uapi_gpio_set_dir(KEY_K2, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KEY_K2, GPIO_LEVEL_HIGH);

	// GPIO_2
	uapi_pin_set_mode(KEY_K3, PIN_MODE_0);
	uapi_gpio_set_dir(KEY_K3, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KEY_K3, GPIO_LEVEL_HIGH);
}


static bool snd_core_process_cmd(c8* sub_cmd, u8 sub_len) {
	c8* tmp;

	if ((tmp = strnstr(sub_cmd, "bt", sub_len))) {				// 蓝牙
		DATA("bt ");
		sub_cmd = sizeof("bt") + tmp;
		sub_len -= sizeof("bt");

		if ((tmp = strnstr(sub_cmd, "onf", sub_len))) {			// 蓝牙 连接/断开
			DATA("onf ");

			tool_pin_gpio_refresh_m(KEY_K1, 7000);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "play", sub_len))) {		// 播放
		DATA("play ");
		sub_cmd = sizeof("play") + tmp;
		sub_len -= sizeof("play");

		if ((tmp = strnstr(sub_cmd, "onf", sub_len))) {			// 播放 开始/暂停/继续
			DATA("onf ");

			tool_pin_gpio_refresh_m(KEY_K1, 200);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "phone", sub_len))) {		// 电话
		DATA("phone ");
		sub_cmd = sizeof("phone") + tmp;
		sub_len -= sizeof("phone");

		if ((tmp = strnstr(sub_cmd, "onf", sub_len))) {			// 电话 接听/挂断
			DATA("onf ");

			tool_pin_gpio_refresh_m(KEY_K1, 200);
		} else if ((tmp = strnstr(sub_cmd, "off", sub_len))) {	// 电话 拒接
			DATA("off ");

			snd_core_click_twice(KEY_K1);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "volume", sub_len))) {		// 音量
		DATA("volume ");
		sub_cmd = sizeof("volume") + tmp;
		sub_len -= sizeof("volume");

		if ((tmp = strnstr(sub_cmd, "+", sub_len))) {				// 音量 ⬆️
			DATA("+ ");

			tool_pin_gpio_refresh_m(KEY_K2, 200);
		} else if ((tmp = strnstr(sub_cmd, "-", sub_len))) {		// 音量 ⬇️
			DATA("- ");

			tool_pin_gpio_refresh_m(KEY_K3, 200);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "chapter", sub_len))) {	// 曲目
		DATA("chapter ");
		sub_cmd = sizeof("chapter") + tmp;
		sub_len -= sizeof("chapter");

		if ((tmp = strnstr(sub_cmd, "+", sub_len))) {				// 曲目 下一首
			DATA("+ ");

			snd_core_click_twice(KEY_K3);
		} else if ((tmp = strnstr(sub_cmd, "-", sub_len))) {		// 曲目 上一首
			DATA("- ");

			snd_core_click_twice(KEY_K2);
		} else {
			return false;
		}
	} else {
		return false;
	}

	DATA("]\n\n");
	return true;
}


void snd_cmd_entry(c8* buff, u16 length) {
	if (is_wating) {
		LOG("\n\tLast snd cmd is processing ...\n\n");
		return;
	}

	is_wating = true;

	c8* substr = strnstr(buff, "snd", length);
	if (substr == NULL) {
		// ERROR("\n\tcmd error: [%s]\n\n", buff);
		is_wating = false;
		return;
	}

	DATA("\n\tGet snd cmd[%u]: [ ", length);
	substr += sizeof("snd");
	length -= sizeof("snd");

	tool_pin_gpio_set_val(0, 1);
	tool_pin_gpio_set_val(1, 1);
	tool_pin_gpio_set_val(2, 1);

	if (false == snd_core_process_cmd(substr, length))
		DATA("ERROR... ]\n\n");

	tool_pin_gpio_set_val(0, 1);
	tool_pin_gpio_set_val(1, 1);
	tool_pin_gpio_set_val(2, 1);

	is_wating = false;
}

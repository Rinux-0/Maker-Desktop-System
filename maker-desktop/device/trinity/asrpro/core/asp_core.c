#include "asp_core.h"

#include "ddef.h"
#include "ttool.h"



static volatile bool is_wating;



void asp_core_init_pin(void) {
	// // GPIO_0
	// uapi_pin_set_mode(0, PIN_MODE_0);
	// uapi_gpio_set_dir(0, GPIO_DIRECTION_OUTPUT);
	// uapi_gpio_set_val(0, GPIO_LEVEL_HIGH);

	// // GPIO_1
	// uapi_pin_set_mode(1, PIN_MODE_0);
	// uapi_gpio_set_dir(1, GPIO_DIRECTION_OUTPUT);
	// uapi_gpio_set_val(1, GPIO_LEVEL_HIGH);

	// // GPIO_2
	// uapi_pin_set_mode(2, PIN_MODE_0);
	// uapi_gpio_set_dir(2, GPIO_DIRECTION_OUTPUT);
	// uapi_gpio_set_val(2, GPIO_LEVEL_HIGH);
}


static bool asp_core_process_cmd(c8* sub_cmd, u8 sub_len) {
	c8* tmp;

	if ((tmp = strnstr(sub_cmd, "tgl", sub_len))) {
		DATA("tgl ");
		sub_cmd = sizeof("tgl") + tmp;
		sub_len -= sizeof("tgl");

		if ((tmp = strnstr(sub_cmd, "onf", sub_len))) {
			DATA("onf ]\n\n");
			sub_cmd = sizeof("onf") + tmp;
			sub_len -= sizeof("onf");

			tool_pin_gpio_refresh_m(0, 7000);
		} else if ((tmp = strnstr(sub_cmd, "st", sub_len))) {
			DATA("st ]\n\n");
			sub_cmd = sizeof("st") + tmp;
			sub_len -= sizeof("st");

			tool_pin_gpio_refresh_m(0, 200);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "+", sub_len))) {
		DATA("+ ");
		sub_cmd = sizeof("+") + tmp;
		sub_len -= sizeof("+");

		if ((tmp = strnstr(sub_cmd, "vol", sub_len))) {
			DATA("vol ]\n\n");
			sub_cmd = sizeof("vol") + tmp;
			sub_len -= sizeof("vol");

			tool_pin_gpio_refresh_m(1, 200);
		} else if ((tmp = strnstr(sub_cmd, "odr", sub_len))) {
			DATA("odr ]\n\n");
			sub_cmd = sizeof("odr") + tmp;
			sub_len -= sizeof("odr");

			asp_core_click_twice(2);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "-", sub_len))) {
		DATA("- ");
		sub_cmd = sizeof("-") + tmp;
		sub_len -= sizeof("-");

		if ((tmp = strnstr(sub_cmd, "vol", sub_len))) {
			DATA("vol ]\n\n");
			sub_cmd = sizeof("vol") + tmp;
			sub_len -= sizeof("vol");

			tool_pin_gpio_refresh_m(2, 200);
		} else if ((tmp = strnstr(sub_cmd, "odr", sub_len))) {
			DATA("odr ]\n\n");
			sub_cmd = sizeof("odr") + tmp;
			sub_len -= sizeof("odr");

			asp_core_click_twice(1);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "rfs", sub_len))) {
		DATA("rfs ]\n\n");
		sub_cmd = sizeof("rfs") + tmp;
		sub_len -= sizeof("rfs");

		asp_core_click_twice(0);
	} else {
		return false;
	}

	return true;
}


void asp_cmd_entry(c8* buff, u16 length) {
	if (is_wating) {
		LOG("\n\tLast asp cmd is processing ...\n\n");
		return;
	}

	is_wating = true;

	c8* substr = strnstr(buff, "asp", length);
	if (substr == NULL) {
		DATA("\n\tcmd error !\n\n");
		is_wating = false;
		return;
	}
	DATA("\n\tGet asp cmd[%u]: [ ", length);
	substr += sizeof("asp");
	length -= sizeof("asp");

	tool_pin_gpio_set_val(0, 1);
	tool_pin_gpio_set_val(1, 1);
	tool_pin_gpio_set_val(2, 1);

	if (false == asp_core_process_cmd(substr, length))
		DATA("ERROR... ]\n\n");

	tool_pin_gpio_set_val(0, 1);
	tool_pin_gpio_set_val(1, 1);
	tool_pin_gpio_set_val(2, 1);

	is_wating = false;
}

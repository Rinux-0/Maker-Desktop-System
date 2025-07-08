#include "lamp_core.h"

#include "ddef.h"
#include "ttool.h"

#include "lamp_pwm.h"


#define LAMP_PWM_PERIOD 250

float lamp_pwm_light_rate = .5f;
float lamp_pwm_warm_rate = .5f;

static volatile bool is_wating;



static void lamp_set_pwm(float light_change, float warm_change) {
	if (fabsf(light_change) > 1e-5f) {
		lamp_pwm_light_rate += light_change;
		if (lamp_pwm_light_rate > 1.f) {
			lamp_pwm_light_rate = 1.f;
		} else if (lamp_pwm_light_rate < 0.f) {
			lamp_pwm_light_rate = 0.f;
		}
	}
	if (fabsf(warm_change) > 1e-5f) {
		lamp_pwm_warm_rate += warm_change;
		if (lamp_pwm_warm_rate > 1.f) {
			lamp_pwm_warm_rate = 1.f;
		} else if (lamp_pwm_warm_rate < 0.f) {
			lamp_pwm_warm_rate = 0.f;
		}
	}

	// 冷
	lamp_pwm_cfg[0].high_time = LAMP_PWM_PERIOD * lamp_pwm_light_rate * (1 - lamp_pwm_warm_rate);
	lamp_pwm_cfg[0].low_time = LAMP_PWM_PERIOD - lamp_pwm_cfg[0].high_time;
	// 暖
	lamp_pwm_cfg[1].high_time = LAMP_PWM_PERIOD * lamp_pwm_light_rate * lamp_pwm_warm_rate;
	lamp_pwm_cfg[1].low_time = LAMP_PWM_PERIOD - lamp_pwm_cfg[1].high_time;

	lamp_pwm_open();	// 重开PWM
}


void lamp_core_init_pin(void) {
	lamp_pwm_init();
	lamp_pwm_open();
}


static bool lamp_core_process_cmd(c8* sub_cmd, u8 sub_len) {
	c8* tmp;

	if ((tmp = strnstr(sub_cmd, "light", sub_len))) {				// 亮度
		DATA("light ");
		sub_cmd = sizeof("light") + tmp;
		sub_len -= sizeof("light");

		if ((tmp = strnstr(sub_cmd, "max", sub_len))) {			// 亮度 最大
			DATA("max ");

			lamp_set_pwm(+2.f, 0.f);
		} else if ((tmp = strnstr(sub_cmd, "min", sub_len))) {	// 亮度 最小
			DATA("min ");

			lamp_set_pwm(-2.f, 0.f);
		} else if ((tmp = strnstr(sub_cmd, "+", sub_len))) {		// 亮度 ⬆️
			DATA("+ ");

			lamp_set_pwm(+.075f, .0f);
		} else if ((tmp = strnstr(sub_cmd, "-", sub_len))) {		// 亮度 ⬇️
			DATA("- ");

			lamp_set_pwm(-.075f, .0f);
		} else {
			return false;
		}
	} else if ((tmp = strnstr(sub_cmd, "hue", sub_len))) {		// 色调
		DATA("hue ");
		sub_cmd = sizeof("hue") + tmp;
		sub_len -= sizeof("hue");

		if ((tmp = strnstr(sub_cmd, "max", sub_len))) {			// 色调 最大（暖）
			DATA("max ");

			lamp_set_pwm(0.f, +2.f);
		} else if ((tmp = strnstr(sub_cmd, "min", sub_len))) {	// 色调 最小（冷）
			DATA("min ");

			lamp_set_pwm(0.f, -2.f);
		} else if ((tmp = strnstr(sub_cmd, "+", sub_len))) {		// 色调 ⬆️
			DATA("+ ");

			lamp_set_pwm(0.f, +.075f);
		} else if ((tmp = strnstr(sub_cmd, "-", sub_len))) {		// 色调 ⬇️
			DATA("- ");

			lamp_set_pwm(0.f, -.075f);
		} else {
			return false;
		}
	} else {
		return false;
	}

	DATA("]\n\n");
	return true;
}


void lamp_cmd_entry(c8* buff, u16 length) {
	static bool tmp;		// ???
	if (is_wating && tmp) {
		LOG("\n\tLast lamp cmd is processing ...\n\n");
		return;
	}

	is_wating = true;

	c8* substr = strnstr(buff, "lamp", length);
	if (substr == NULL) {
		DATA("\n\tcmd error !\n\n");
		is_wating = false;
		return;
	}

	DATA("\n\tGet lamp cmd[%u]: [ ", length);
	substr += sizeof("lamp");
	length -= sizeof("lamp");

	if (false == lamp_core_process_cmd(substr, length))
		DATA("ERROR... ]\n\n");

	is_wating = false;
	tmp = true;
}

#include "my_tool_pwm.h"

pwm_config_t mypwm_config = {
		50,     //低电平
		200,    //高电平
		0,
		0,
		true
};

//gpio 0——pwm 0
//gpio 1——pwm 1
//gpio 2——pwm 2
//gpio 3——pwm 3
//gpio 4——pwm 4
//gpio 5——pwm 5
//gpio 6——pwm 6
//gpio 7——pwm 7
//gpio 8——pwm 0
//gpio 9——pwm 1
//gpio 10——pwm 2
//gpio 11——pwm 3
//gpio 12——pwm 4
void mypwm_init(pin_t pin) {
	uapi_pin_set_mode(pin, 1);
	uapi_pwm_deinit();
	uapi_pwm_init();
}

void mypwm_start(pin_t pin, uint32_t T, float duty, pwm_config_t config) {
	uint8_t channel_id;
	if (pin <= 7) {
		channel_id = pin;
	} else {
		channel_id = pin - 8;
	}

	config.high_time = T * duty;
	config.low_time = T * (1 - duty);

	uapi_pwm_open(channel_id, &config);
	uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
	uapi_pwm_start_group(PWM_GROUP_ID);
}

void mypwm_setconfig(pwm_config_t* config, uint32_t HIGHTIME, uint32_t LOWTIME, uint32_t OFFSETTIEM, uint16_t CYCLE, bool REPEAT) {
	config->high_time = HIGHTIME;
	config->low_time = LOWTIME;
	config->offset_time = OFFSETTIEM;
	config->cycles = CYCLE;
	config->repeat = REPEAT;
}

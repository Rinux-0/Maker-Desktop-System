#include "lamp_pwm.h"

#include "ddef.h"
#include "ttool.h"

#include <pwm.h>



#define LAMP_PWM_GROUP_ID        0
#define LAMP_PWM_COLD_CHANNEL    LAMP_PWM_COLD_PIN
#define LAMP_PWM_WARM_CHANNEL    LAMP_PWM_WARM_PIN

static u8 lamp_channel_id[2] = { LAMP_PWM_COLD_CHANNEL, LAMP_PWM_WARM_CHANNEL };
pwm_config_t lamp_pwm_cfg[2] = {
	{			// cold
		125,	//低电平
		75,		//高电平
		0, 0,
		true
	}, {		// warm
		125,	//低电平
		75,		//高电平
		0, 0,
		true
	}
};



void lamp_pwm_init(void) {
	uapi_pin_set_mode(LAMP_PWM_COLD_PIN, 1);
	uapi_pin_set_mode(LAMP_PWM_WARM_PIN, 1);

	uapi_pwm_deinit();
	uapi_pwm_init();
}


void lamp_pwm_open(void) {
	for (u8 i = 0; i < 2; i++)
		uapi_pwm_open(lamp_channel_id[i], &lamp_pwm_cfg[i]);

	uapi_pwm_set_group(LAMP_PWM_GROUP_ID, lamp_channel_id, 2);

	uapi_pwm_start_group(LAMP_PWM_GROUP_ID);
}

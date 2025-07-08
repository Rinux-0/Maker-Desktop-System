#pragma once
#ifndef _LAMP_PWM_H_
#define _LAMP_PWM_H_



#include "ddef.h"
#include <pwm.h>

#define LAMP_PWM_COLD_PIN   5
#define LAMP_PWM_WARM_PIN   1

extern pwm_config_t lamp_pwm_cfg[2];

void lamp_pwm_init(void);
void lamp_pwm_open(void);



#endif // _LAMP_PWM_H_

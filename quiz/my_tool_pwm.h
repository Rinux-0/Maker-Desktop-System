#ifndef _MY_TOOL_PWM_H_
#define _MY_TOOL_PWM_H_

#include  "my_def.h"
#include  "my_tool.h"
#include "pinctrl.h"
#include "pwm.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include  "main.h"

#define PWM_GROUP_ID 0

extern pwm_config_t mypwm_config;
void mypwm_init(pin_t pin);
void mypwm_start(pin_t pin, uint32_t T, float duty, pwm_config_t config);
void mypwm_setconfig(pwm_config_t* config, uint32_t HIGHTIME, uint32_t LOWTIME, uint32_t OFFSETTIEM, uint16_t CYCLE, bool REPEAT);

#endif

#ifndef _MY_TOOL_ADC_H_
#define _MY_TOOL_ADC_H_

#include  "my_def.h"
#include  "my_tool.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "adc.h"
#include "adc_porting.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "main.h"

#define ADCDATANUM 100

extern adc_scan_config_t config;
extern float adc_data;

void myadc_callback(uint8_t ch, uint32_t* buffer, uint32_t length, bool* next);
void myadc_init(void);
void myadc_loop(pin_t pin);

#endif

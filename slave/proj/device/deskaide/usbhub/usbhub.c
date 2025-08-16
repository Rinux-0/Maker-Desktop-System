#include "usbhub.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"
#include "usbhub_def.h"

#include <adc.h>
#include <adc_porting.h>
#include <cmsis_os2.h>

// c2-g9
// c5-g12

static adc_scan_config_t config = {
	.type = 0,
	.freq = 1,
};
static usb_output_t usb_output[2];



const usb_output_t* usbhub_get_output(void) {
	return usb_output;
}


//adc回调函数
static void usbhub_adc_cb(u8 channel, u32* buffer, u32 length, bool* next) {
	unused(next);

	u8 index;

	// if (channel != ADC_CHANNEL_0 && channel != ADC_CHANNEL_5) {
	// 	ERROR("Invalid channel %d", channel);
	// 	return;
	// }
	if (channel == ADC_CHANNEL_0) {
		index = 0;
	} else if (channel == ADC_CHANNEL_5) {
		index = 1;
	} else {
		ERROR("Invalid channel %d", channel);
		return;
	}

	usb_output[index].current_a = (buffer[length - 1] / 1000.f - 3.3f / 2) / .264f;
	usb_output[index].current_a = fabsf(usb_output[index].current_a);
}


static void usbhub_init(void) {
	uapi_pin_set_mode(7, 0);
	uapi_gpio_set_dir(7, GPIO_DIRECTION_INPUT);
	uapi_pin_set_pull(7, 0);

	uapi_pin_set_mode(12, 0);
	uapi_gpio_set_dir(12, GPIO_DIRECTION_INPUT);
	uapi_pin_set_pull(12, 0);

	uapi_adc_init(ADC_CLOCK_500KHZ);
	uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);

	tool_timer_start_m(2, 1000 * 0.25, NULL);
}


static void usbhub_oneloop(void) {
	tool_delay_m(1);

	static bool now = false;
	if (now == g_time_wait_0s25)
		return;
	now = g_time_wait_0s25;

	uapi_adc_auto_scan_ch_enable(ADC_CHANNEL_0, config, usbhub_adc_cb);
	uapi_adc_auto_scan_ch_disable(ADC_CHANNEL_0);

	uapi_adc_auto_scan_ch_enable(ADC_CHANNEL_5, config, usbhub_adc_cb);
	uapi_adc_auto_scan_ch_disable(ADC_CHANNEL_5);
}


static void usbhub_exit(void) {}



static void* usbhub(const c8* arg) {
	unused(arg);

	usbhub_init();
	while (1)
		usbhub_oneloop();
	usbhub_exit();

	return NULL;
}


void usbhub_entry(void) {
	osThreadAttr_t attr = {
		.name = "usbhub",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)usbhub, NULL, &attr)) {
		ERROR("Failed to create usbhub sub_thread");
	}
}

#include <tcxo.h>		// 晶振相关
#include <uart.h>		// 串口相关
#include "util_tool.h"

#include "../comm/uart/uart_cfg_def.h"



void sleep_ms(u32 time_ms) {
	uapi_tcxo_delay_ms(time_ms);
}
void sleep_us(u32 time_us) {
	uapi_tcxo_delay_us(time_us);
}


void pin_voltage_refresh(pin_t pin, u32 time_us) {
	uapi_gpio_toggle(pin);
	sleep_us(time_us);
	uapi_gpio_toggle(pin);
}


void led_init(void) {
	uapi_pin_set_mode(LED_PIN, PIN_MODE_0);	// GPIO
	uapi_gpio_set_dir(LED_PIN, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN, GPIO_LEVEL_HIGH);
}
void led_on(void) {
	uapi_gpio_set_val(LED_PIN, GPIO_LEVEL_LOW);
}
void led_off(void) {
	uapi_gpio_set_val(LED_PIN, GPIO_LEVEL_HIGH);
}
void led_toggle(void) {
	uapi_gpio_toggle(LED_PIN);
}

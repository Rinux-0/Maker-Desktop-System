#include "util_def.h"
#include "util_tool.h"

#include <gpio.h>
#include <hal_watchdog.h>
#include <pinctrl.h>
// #include <tcxo.h>
#include <unistd.h>


static hal_watchdog_funcs_t* watchdog;
#if defined(CONFIG_DEVICE_KEYBOARD)
#define LED_PIN 12
#elif defined(CONFIG_DEVICE_KEYPAD)
#define LED_PIN 13
#else
#define LED_PIN PIN_NONE
#error "暂未开发其他设备的LED控制"
#endif



// static void tcxo_init(void) {
// 	uapi_tcxo_deinit();
// 	uapi_tcxo_init();
// }
void mdelay(u32 time_ms) {
	// uapi_tcxo_delay_ms(time_ms);
	sleep(time_ms);
}
void udelay(u32 time_us) {
	// uapi_tcxo_delay_us(time_us);
	usleep(time_us);
}



static void watchdog_init(u32 timeout_ms) {
	watchdog = hal_watchdog_get_funcs();
	watchdog->deinit();
	watchdog->init();
	watchdog->disable();
	watchdog->enable(HAL_WDT_MODE_RESET);	// 超时复位
	watchdog->set_attr(timeout_ms);			// ms
}
void watchdog_kick(void) {
	watchdog->kick();
}



static void gpio_init(void) {
	uapi_gpio_deinit();
	uapi_gpio_init();
}
void gpio_refresh(pin_t pin, u32 time_us) {
	uapi_gpio_toggle(pin);
	udelay(time_us);
	uapi_gpio_toggle(pin);
}



static void led_init(void) {
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



void util_tool_init(void) {
	// tcxo_init();
	watchdog_init(3000);
	gpio_init();

	led_init();
}

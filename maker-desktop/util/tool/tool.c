#include "def.h"
#include "tool.h"

// #include <cmsis_os2.h>
#include <soc_osal.h>
#include <gpio.h>
#include <hal_watchdog.h>
#include <pinctrl.h>



static hal_watchdog_funcs_t* watchdog;



void m_sleep(u16 time_ms) {
	// osDelay(time_ms);
	osal_msleep(time_ms);
}
void u_delay(u16 time_us) {
	for (u16 i=0; i<time_us*20; i++);
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
	u_delay(time_us);
	uapi_gpio_toggle(pin);
}


#if LED_PIN != PIN_NONE
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
#else
void led_init(void) {}
#endif


void util_tool_init(void) {
	watchdog_init(3000);
	gpio_init();

	led_init();
}

#include "ddef.h"
#include "ttool.h"

#include <gpio.h>
#include <hal_watchdog.h>
#include <pinctrl.h>
#include <tcxo.h>



u64 time_debug;
static hal_watchdog_funcs_t* watchdog;



static void tool_tcxo_init(void) {
	uapi_tcxo_init();
}
u64 tool_tcxo_get_m(void) {
	return uapi_tcxo_get_ms();
}
bool tool_tcxo_is_timeout(u64 timeout_ms, u64 start_time_ms) {
	return uapi_tcxo_get_ms() - start_time_ms > timeout_ms;
}



void tool_sleep_m(u16 time_ms) {
	osal_msleep(time_ms);
	// osDelay(time_ms);
}
void tool_delay_u(u16 time_us) {
	for (u16 i = 0; i < time_us * 20; i++);
}



static void tool_watchdog_init(u32 timeout_ms) {
	watchdog = hal_watchdog_get_funcs();
	watchdog->deinit();
	watchdog->init();
	watchdog->disable();
	watchdog->enable(HAL_WDT_MODE_RESET);	// 超时复位
	watchdog->set_attr(timeout_ms);			// ms
}
void tool_watchdog_kick(void) {
	watchdog->kick();
}



void tool_pin_gpio_init(pin_t pin, gpio_direction_t dir, gpio_level_t level) {
	uapi_pin_set_mode(pin, PIN_MODE_0);
	uapi_gpio_set_dir(pin, dir);
	uapi_gpio_set_val(pin, level);
}
void tool_pin_gpio_set_val(pin_t pin, gpio_level_t level) {
	uapi_gpio_set_val(pin, level);

	DATA("GPIO[%d] = [%d]\n", pin, level);
}
void tool_pin_gpio_toggle(pin_t pin) {
	uapi_gpio_toggle(pin);
}
void tool_pin_gpio_refresh(pin_t pin, u32 time_us) {
	uapi_gpio_toggle(pin);
	tool_delay_u(time_us);
	uapi_gpio_toggle(pin);
}



static void tool_gpio_init(void) {
	uapi_gpio_deinit();
	uapi_gpio_init();
}



static void tool_led_init(void) {
	uapi_pin_set_mode(LED_PIN_RUN, PIN_MODE_0);
	uapi_gpio_set_dir(LED_PIN_RUN, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN_RUN, GPIO_LEVEL_HIGH);

	uapi_pin_set_mode(LED_PIN_SLE, PIN_MODE_0);
	uapi_gpio_set_dir(LED_PIN_SLE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN_SLE, GPIO_LEVEL_HIGH);
}
void tool_led_run_toggle(void) {
	uapi_gpio_toggle(LED_PIN_RUN);
}



void tool_init(void) {
	tool_tcxo_init();
	tool_gpio_init();
	tool_watchdog_init(1024 * 0.5);
	tool_led_init();

	tool_timer_init();
}


void tool_exit(void) {
	uapi_tcxo_deinit();
	uapi_gpio_deinit();
	watchdog->deinit();

	tool_timer_exit();
}

#include "my_tool_gpio.h"

struct key keys[4] = { 0 };

//mode=0:in  mode=1:out
void mygpio_init(pin_t pin, pin_mode_t mode) {
	if (pin == 4) {
		uapi_pin_set_mode(pin, 2);
	} else if (pin == 5) {
		uapi_pin_set_mode(pin, 4);
	} else {
		uapi_pin_set_mode(pin, 0);
	}

	if (mode == 0) {
		uapi_gpio_set_dir(pin, 0);
	} else {
		uapi_gpio_set_dir(pin, 1);
	}
}

//外部中断回调
void mygpio_interrupt_callback(pin_t pin, uintptr_t param) {
	UNUSED(pin);
	UNUSED(param);
	//使用：
	//uapi_pin_set_mode(CONFIG_BUTTON_PIN, HAL_PIO_FUNC_GPIO);  gpio模式
	// uapi_pin_set_pull(CONFIG_BUTTON_PIN, PIN_PULL_TYPE_UP);  上下拉
	// uapi_gpio_set_dir(CONFIG_BUTTON_PIN, GPIO_DIRECTION_INPUT);  输入
	// uapi_gpio_register_isr_func(CONFIG_BUTTON_PIN, GPIO_INTERRUPT_DEDGE, gpio_callback_func);  注册
}

void key_init(void) {
	mygpio_init(KEY1, 0);
	mygpio_init(KEY2, 0);
	mygpio_init(KEY3, 0);
	mygpio_init(KEY4, 0);

	//  PIN_PULL_TYPE_DISABLE = 0,
	// 	PIN_PULL_TYPE_DOWN = 1,
	// 	PIN_PULL_TYPE_STRONG_UP = 2,
	// 	PIN_PULL_TYPE_UP = 3,
	// 	PIN_PULL_MAX = 4
	uapi_pin_set_pull(KEY1, 1);
	uapi_pin_set_pull(KEY2, 1);
	uapi_pin_set_pull(KEY3, 1);
	uapi_pin_set_pull(KEY4, 1);
}

static uint8_t time[4] = { 0 };

//test
float pwmduty=0;

char tx[30] = " ";
void keyscan(void) {
	keys[0].value = uapi_gpio_get_val(KEY1);
	keys[1].value = uapi_gpio_get_val(KEY2);
	keys[2].value = uapi_gpio_get_val(KEY3);
	keys[3].value = uapi_gpio_get_val(KEY4);

	for (int i = 0;i < 4;i++) {
		switch (keys[i].state) {
		case 0:if (keys[i].value == 1) {
			keys[i].state = 1;
		}break;
		case 1:if (keys[i].value == 1) {
			keys[i].state = 2;
		}break;
		case 2:if (keys[i].value == 1) {
			keys[i].time++;
		} else {
			if (keys[i].time >= 100) {
				keys[i].kind = 'l';
			} else {
				keys[i].kind = 's';
			}
			keys[i].time = 0;
			keys[i].state = 0;
		}
		}
	}

	if (keys[0].kind == 's') {
		// time[0]++;
		// sprintf(tx, "key1:%d", time[0]);
		// OLED_ShowString(1, 1, tx);

		pwmduty += 0.01;
		keys[0].kind = 'n';
	}
	if (keys[1].kind == 's') {
		// time[1]++;
		// sprintf(tx, "key2:%d", time[1]);
		// OLED_ShowString(2, 1, tx);

		pwmduty -= 0.01;
		keys[1].kind = 'n';
	}
	if (keys[2].kind == 's') {
		// time[2]++;
		// sprintf(tx, "key3:%d", time[2]);
		// OLED_ShowString(3, 1, tx);

		pwmduty += 0.1;
		keys[2].kind = 'n';
	}
	if (keys[3].kind == 's') {
		// time[3]++;
		// sprintf(tx, "key4:%d", time[3]);
		// OLED_ShowString(4, 1, tx);

		pwmduty -= 0.1;
		keys[3].kind = 'n';
	}
}

#include "main.h"

static void quiz_init(void) {
	my_dog_init(8);

	//led测试
	//mygpio_init(LEDPIN, 1);

	//Oled测试
	// OLED_Init();
	// OLED_ShowNum(1, 1, 1, 1);

	//定时器测试
	// tool_timer_init();
	// tool_timer_start_m(1, 1000, mytimer_callback);

	//按键扫描测试
	// tool_timer_init();
	// key_init();
	// tool_timer_start_m(0, 10, mytimer_callback);

	//pwm测试
	// mypwm_init(10);
	// mypwm_start(10, 1000, 0.3, mypwm_config);

	//adc测试
	// myadc_init();

	//RGB测试
	// myspi_init();

	//uart测试
	myuart_init(0);

}


static void quiz_loop(void) {
	while (1) {
		my_dog_kick();

		usleep(10);

		//RGB测试
		// for (int i = 0;i < 256;i++) {
		// 	RGB_rainbow(i);
		// 	usleep(10);
		// }

		//pwm测试
		// usleep(1000000);
		// mypwm_start(10, 1000, 0.3, mypwm_config);
		// usleep(1000000);
		// mypwm_start(10, 1000, 0.7, mypwm_config);

		//adc测试
		// mypwm_start(10, 1000, pwmduty, mypwm_config);
		// char tx[30] = " ";
		// sprintf(tx, "duty:%.2f", pwmduty);
		// OLED_ShowString(2, 1, tx);
		// myadc_loop(7);
	}
}


static void* quiz(const c8* arg) {
	unused(arg);

	quiz_init();
	quiz_loop();

	return NULL;
}


static void quiz_entry(void) {
	osThreadAttr_t attr = {
		.name = "quiz",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x3000,
		.priority = (osPriority_t)28,
	};

	if (NULL == osThreadNew((osThreadFunc_t)quiz, NULL, &attr)) {
		ERROR("Failed to create quiz thread");
	}
}


app_run(quiz_entry);

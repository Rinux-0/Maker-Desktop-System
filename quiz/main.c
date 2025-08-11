#include "main.h"

static void quiz_init(void) {
	my_dog_init(8);

	//Oled测试
	OLED_Init();
	// OLED_ShowNum(1, 1, 1, 1);

	//定时器测试
	// tool_timer_init();
	// tool_timer_start_m(1, 1000, mytimer_callback);

	//按键扫描测试
	tool_timer_init();
	key_init();
	tool_timer_start_m(0, 10, mytimer_callback);

	//adc测试
	myadc_init();

	//RGB测试
	myspi_init();
	tool_timer_start_m(3, 1000, mytimer_callback);
	tool_timer_start_m(1, 1, mytimer_callback);
	tool_timer_start_m(2, 2, mytimer_callback);
}


static void quiz_loop(void) {
	while (1) {
		my_dog_kick();

		// usleep(10);
		//adc测试
		// mypwm_start(10, 1000, pwmduty, mypwm_config);
		// char tx[30] = " ";
		// sprintf(tx, "duty:%.2f", pwmduty);
		// OLED_ShowString(2, 1, tx);
		myadc_loop(7);

		// switch (RGB_state) {
		// case 0:break;
		// case 1:RGB_singlershow(RGB_RED);break;
		// case 2:RGB_singlershow(RGB_GREEN);break;
		// case 3:RGB_singlershow(RGB_BLUE);break;
		// case 4:
			// if (RGB_state != 4)break;
			// RGB_color_now++;
			// switch (RGB_color_now) {
			// case 0:RGB_singlershow(RGB_RED);sleep(1);break;
			// case 1:RGB_singlershow(RGB_GREEN);sleep(1);break;
			// case 2:RGB_singlershow(RGB_BLUE);sleep(1);RGB_color_now = 0;break;
			// }break;
		// }
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

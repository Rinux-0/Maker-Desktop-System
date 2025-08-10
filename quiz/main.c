#include  "my_def.h"
#include  "my_tool.h"

// 其他头文件

#include <app_init.h>
#include <cmsis_os2.h>



static void quiz_init(void) {
	my_dog_init(8);

	// ...
}


static void quiz_loop(void) {
	while (1) {
		my_dog_kick();

		// ...
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

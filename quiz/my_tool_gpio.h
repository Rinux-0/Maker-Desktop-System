#ifndef _MY_TOOL_GPIO_H_
#define _MY_TOOL_GPIO_H_

#include "my_def.h"
#include "my_tool.h"
#include "main.h"
#include "my_tool_timer.h"
#include <chip_core_irq.h>
#include "OLED.h"

#define KEY1 6
#define KEY2 3
#define KEY3 13
#define KEY4 14
#define LEDPIN 10

struct key
{
	bool value;
	char kind;
	unsigned int time;
	unsigned int state;
};
extern struct key keys[];

//test
extern float pwmduty;

void mygpio_init(pin_t pin, pin_mode_t mode);
void mygpio_interrupt_callback(pin_t pin, uintptr_t param);
void key_init(void);
void keyscan(void);

#endif

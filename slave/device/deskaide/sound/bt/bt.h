#pragma once
#ifndef _BT_H_
#define _BT_H_



#include "ddef.h"
#include "ttool.h"

#define SND_BT_GPIO_PIN 10

void snd_bt_init_pin(void);
bool snd_bt_is_conn(void);
void snd_bt_gpio_int_handler(pin_t pin, uintptr_t param);



#endif // _BT_H_

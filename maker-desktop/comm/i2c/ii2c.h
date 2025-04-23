#ifndef _MYI2C_H_
#define _MYI2C_H_



#include "ddef.h"
#include "ii2c_def.h"
#include <pinctrl.h>
#include <gpio.h>

void i2c_pin_set_dir(pin_t pin, gpio_direction_t io_dir);
void i2c_pin_set_outval(pin_t pin, bool outval);
gpio_level_t i2c_pin_get_inval(pin_t pin);

void i2c_start(void);
void i2c_stop(void);

u8 i2c_wait_ack(void);
void i2c_send_ack(void);
void i2c_send_not_ack(void);

void i2c_send_byte(u8 byte_sended);
u8 i2c_receive_byte(void);

void i2c_init(void);
void i2c_oneloop(void);
void i2c_exit(void);



#endif // _MYI2C_H_

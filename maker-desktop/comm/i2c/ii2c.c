#include "ii2c.h"
#include "ii2c_def.h"

#include "ddef.h"
#include "ttool.h"

#include <gpio.h>
#include <i2c.h>
#include <pinctrl.h>



//
void i2c_pin_set_dir(pin_t pin, gpio_direction_t io_dir) {
	uapi_gpio_set_dir(pin, io_dir);
}


//
void i2c_pin_set_outval(pin_t pin, bool outval) {
	uapi_gpio_set_val(pin, outval);
}


//
gpio_level_t i2c_pin_get_inval(pin_t pin) {
	return uapi_gpio_get_val(pin);
}


//
void i2c_start(void) {
	i2c_pin_set_outval(PIN_SDA, 1);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SCL, 1);
	tool_delay_u(5);

	i2c_pin_set_outval(PIN_SDA, 0);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SCL, 0);
	tool_delay_u(5);
}


//
void i2c_stop(void) {
	i2c_pin_set_outval(PIN_SCL, 0);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SDA, 0);
	tool_delay_u(5);

	i2c_pin_set_outval(PIN_SCL, 1);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SDA, 1);
	tool_delay_u(5);

}


//
u8 i2c_wait_ack(void) {
	u8 time_error = 5;

	i2c_pin_set_dir(PIN_SDA, GPIO_IN);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SCL, 1);
	tool_delay_u(5);

	while (i2c_pin_get_inval(PIN_SDA)) {
		time_error--;
		tool_delay_u(5);
		if (0 == time_error) {
			i2c_pin_set_dir(PIN_SDA, GPIO_OUT);
			i2c_stop();
			return RET_ERROR;
		}
	}

	i2c_pin_set_outval(PIN_SCL, 0);
	i2c_pin_set_dir(PIN_SDA, GPIO_OUT);
	tool_delay_u(5);

	return RET_SUCCESS;
}


//
void i2c_send_ack(void) {
	i2c_pin_set_outval(PIN_SDA, 0);
	tool_delay_u(10);
	i2c_pin_set_outval(PIN_SCL, 1);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SCL, 0);
	tool_delay_u(5);
}


//
void i2c_send_not_ack(void) {
	i2c_pin_set_outval(PIN_SDA, 1);
	tool_delay_u(10);
	i2c_pin_set_outval(PIN_SCL, 1);
	tool_delay_u(5);
	i2c_pin_set_outval(PIN_SCL, 0);
	tool_delay_u(5);
}


//
void i2c_send_byte(u8 byte) {
	u8 i = 8;
	while (i--) {
		i2c_pin_set_outval(PIN_SCL, 0);
		tool_delay_u(5);
		i2c_pin_set_outval(PIN_SDA, byte & 0x80);
		tool_delay_u(5);
		byte *= 2;
		tool_delay_u(5);
		i2c_pin_set_outval(PIN_SCL, 1);
		tool_delay_u(5);
	}

	i2c_pin_set_outval(PIN_SCL, 0);
	tool_delay_u(5);
}


//
u8 i2c_receive_byte(void) {
	u8 byte_received = 0;

	i2c_pin_set_dir(PIN_SDA, GPIO_IN);

	u8 i = 8;
	while (i--) {
		byte_received *= 2;
		i2c_pin_set_outval(PIN_SCL, 0);
		tool_delay_u(10);
		i2c_pin_set_outval(PIN_SCL, 1);
		tool_delay_u(5);
		byte_received |= i2c_pin_get_inval(PIN_SDA);
	}

	i2c_pin_set_outval(PIN_SCL, 0);
	tool_delay_u(5);
	i2c_pin_set_dir(PIN_SDA, GPIO_OUT);

	return byte_received;
}


void i2c_write(u8 addr, const u8* data, u16 length) {

}

//
void i2c_init(void) {
	uapi_pin_set_mode(PIN_SDA, 0);
	uapi_pin_set_mode(PIN_SCL, 0);

	i2c_pin_set_dir(PIN_SCL, GPIO_OUT);
	i2c_pin_set_dir(PIN_SDA, GPIO_OUT);

	i2c_pin_set_outval(PIN_SDA, 1);
	i2c_pin_set_outval(PIN_SCL, 1);

	uapi_pin_set_pull(PIN_SDA, PIN_PULL_TYPE_UP);
	uapi_pin_set_pull(PIN_SCL, PIN_PULL_TYPE_UP);
}


void i2c_oneloop(void) {}


void i2c_exit(void) {}

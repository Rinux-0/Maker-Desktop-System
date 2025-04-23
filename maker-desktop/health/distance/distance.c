#include "distance.h"

#include "ddef.h"
#include "ii2c.h"
#include "ttool.h"



u16 distance_get_value(void) {
	u16 data;

	i2c_start();

	i2c_send_byte(0xE8);
	i2c_wait_ack();
	i2c_send_byte(0x10);
	i2c_wait_ack();
	i2c_send_byte(0xB0);
	i2c_wait_ack();

	i2c_stop();
	tool_delay_u(40000);
	i2c_start();

	i2c_send_byte(0xE8);
	i2c_wait_ack();
	i2c_send_byte(0x02);
	i2c_wait_ack();

	i2c_stop();
	i2c_start();

	i2c_send_byte(0xE9);
	i2c_wait_ack();
	data = i2c_receive_byte();
	i2c_send_ack();
	data = data * 256 + i2c_receive_byte();
	i2c_send_not_ack();

	i2c_stop();

	return data;
}

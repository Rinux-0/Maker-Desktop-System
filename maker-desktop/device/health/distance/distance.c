#include "distance.h"

#include "ddef.h"
#include "ttool.h"

#include "ii2c.h"



u16 distance_data_now;
u16 distance_data_last;



static void distance_write_get_req(void) {
	i2c_start();
	i2c_send_byte(0xE8);
	i2c_wait_ack();
	i2c_send_byte(0x10);
	i2c_wait_ack();
	i2c_send_byte(0xB0);
	i2c_wait_ack();
	i2c_stop();

	tool_sleep_m(40);

	i2c_start();
	i2c_send_byte(0xE8);
	i2c_wait_ack();
	i2c_send_byte(0x02);
	i2c_wait_ack();
	i2c_stop();

	i2c_start();
	i2c_send_byte(0xE9);
	i2c_wait_ack();
	distance_data_now = i2c_receive_byte();
	i2c_send_ack();
	distance_data_now = (distance_data_now << 8) + i2c_receive_byte();
	i2c_send_not_ack();
	i2c_stop();
}


void distance_init(void) {}


void distance_oneloop(void) {
	distance_write_get_req();

	if (distance_data_last != distance_data_now) {
		DATA("distance: %d\n", distance_data_now);
		distance_data_last = distance_data_now;
	}
}


void distance_exit(void) {}

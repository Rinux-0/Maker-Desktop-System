#include "distance.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "ii2c.h"

#include <cmsis_os2.h>



static u8 str_distance[5];
static u16 distance_data_now;
static u16 distance_data_last;



/// @todo 注释
static void distance_write_get_req(void) {
	//
	i2c_start();
	i2c_send_byte(0xE8);
	i2c_wait_ack();
	i2c_send_byte(0x10);
	i2c_wait_ack();
	i2c_send_byte(0xB0);
	i2c_wait_ack();
	i2c_stop();

	tool_sleep_m(40);

	//
	i2c_start();
	i2c_send_byte(0xE8);
	i2c_wait_ack();
	i2c_send_byte(0x02);
	i2c_wait_ack();
	i2c_stop();

	//
	i2c_start();
	i2c_send_byte(0xE9);
	i2c_wait_ack();
	distance_data_now = i2c_receive_byte();
	i2c_send_ack();
	distance_data_now = (distance_data_now << 8) + i2c_receive_byte();
	i2c_send_not_ack();
	i2c_stop();
}


static void distance_init(void) {}


static void distance_oneloop(void) {
	tool_sleep_m(1);

	distance_write_get_req();

	if (distance_data_last != distance_data_now) {
		sprintf((c8*)str_distance, "d%03d", distance_data_now);
		sle_write(receiver, str_distance, sizeof(str_distance) - 1);

		DATA("\n\tdistance: %d\n\n", distance_data_now);
		distance_data_last = distance_data_now;
	}
}


static void distance_exit(void) {}



static void* distance(const c8* arg) {
	unused(arg);

	distance_init();
	while (1)
		distance_oneloop();
	distance_exit();

	return NULL;
}


void distance_entry(void) {
	osThreadAttr_t attr = {
		.name = "distance",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)distance, NULL, &attr)) {
		ERROR("Failed to create distance sub_thread");
	}
}

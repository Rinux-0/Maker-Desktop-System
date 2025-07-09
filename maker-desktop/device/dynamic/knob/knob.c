#include "knob.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"
#include "./core/knob_core.h"

#include <cmsis_os2.h>



static const u8 data_const_head[] = {
	0x5A, 0xA5, 0x04, 0x03, 0xFF, 0x41,
	0x00, 0x5A, 0xA5, 0x0B, 0x03, /*都自定义为0*/0x00
};
static const u8 cmd_const_head[] = {
	0x5A, 0xA5, 0x07, 0x03, 0x00
};
static const u8 cmd_const_body[][5] = {	// operate_mode
	{ 0x01, 0x00, 0x02, 0xB0, 0x01 },	// 0. 问 操作模式
	{ 0x02, 0x00, 0x02, 0x40, 0x01 },	// 1. 滚动(鼠标)
	{ 0x03, 0x00, 0x02, 0x11, 0xC1 },	// 2. 音量
	{ 0x04, 0x00, 0x02, 0xA0, 0x00 },	// 3. 曲目
	{ 0x05, 0x00, 0x02, 0xF1, 0xC0 },	// 4. 灯亮
	{ 0x06, 0x00, 0x02, 0x01, 0xC0 },	// 5. 灯色
	{ 0x07, 0x00, 0x02, 0x50, 0x00 }	// 6. 切换(Tab)
};

static s8 cmd_operate_mode;
static volatile bool is_wating;



static void knob_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;
	// DATA("len: %d\n%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",
	// 	length,
	// 	buff[0], buff[1], buff[2], buff[3], buff[4], buff[5],
	// 	buff[6], buff[7], buff[8], buff[9], buff[10], buff[11]
	// );

	// 包长
	if (length != 21)
		return;

	// 包头
	for (u8 i = 0;i < sizeof(data_const_head); i++)
		if (buff[i] != data_const_head[i]) {
			ERROR("\n\tlen: %d, i:%d\n\n", length, i);
			return;
		}

	// DATA("[%02x]\n", buff[16]);

	/// @todo 消抖

	switch (cmd_operate_mode) {
	default:			ERROR("\n\tknob: error: operate_mode = %d\n\n", cmd_operate_mode);
	break;case 0x00:	cmd_operate_mode = buff[16] + 1;
	break;case 0x01:	knob_core_mouse_scroll(buff[16]);
	break;case 0x02:	knob_core_volume(buff[16]);
	break;case 0x03:	knob_core_music(buff[16]);
	break;case 0x04:	knob_core_lamp_light(buff[16]);
	break;case 0x05:	knob_core_lamp_hue(buff[16]);
	break;case 0x06:	knob_core_tab(buff[16]);
	}

	is_wating = false;

	// LOG("");
}


static void knob_write_cmd(s8 operate_mode) {
	static u8 recurse_depth = 0;
	if (recurse_depth >= 2)
		return;

	recurse_depth++;

	u8 cmd[10];

	for (u8 i = 0; i < 5; i++) {
		cmd[i] = cmd_const_head[i];
		cmd[i + 5] = cmd_const_body[operate_mode][i];
	}

	uart_write(UART_BUS_ID(1), cmd, sizeof(cmd));
	is_wating = true;

	bool strt = g_time_wait_0s1;
	while (is_wating) {
		tool_sleep_m(1);
		if (strt != g_time_wait_0s1) {
			// DATA("\n\tknob: error_timeout\n\n");
			break;
		}
	}

	if (is_wating == false && cmd_operate_mode > 0)
		knob_write_cmd(cmd_operate_mode);

	recurse_depth--;

	// LOG("");
}


static void knob_init(void) {
	uart_set_r_cb(UART_BUS_ID(1), knob_uart_r_int_handler);
}


static void knob_oneloop(void) {
	tool_sleep_m(1);

	knob_write_cmd(0);

	cmd_operate_mode = 0;
}


static void knob_exit(void) {}



static void* knob(const c8* arg) {
	unused(arg);

	knob_init();
	while (1)
		knob_oneloop();
	knob_exit();

	return NULL;
}


void knob_entry(void) {
	osThreadAttr_t attr = {
		.name = "knob",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)knob, NULL, &attr)) {
		ERROR("Failed to create knob sub_thread");
	}
}

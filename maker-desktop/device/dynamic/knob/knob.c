#include "knob.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"
#include "./core/knob_core.h"



static const u8 data_const_head[] = {
	0x5A, 0xA5, 0x04, 0x03, 0xFF, 0x41,
	0x00, 0x5A, 0xA5, 0x0B, 0x03, '\0'
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
	{ 0x06, 0x00, 0x02, 0x01, 0xC0 },	// 5. 屏亮
	{ 0x07, 0x00, 0x02, 0x50, 0x00 }	// 6. 切换(Tab)
};

static s8 cmd_operate_mode = -1;
static s8 data = -1;
static bool is_wating;
// static bool knob_is_bad;


void knob_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff = (u8*)buffer;
	DATA("len: %d\n%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",
		length,
		buff[0], buff[1], buff[2], buff[3], buff[4], buff[5],
		buff[6], buff[7], buff[8], buff[9], buff[10], buff[11]
	);

	for (u8 i = 0;i < 11; i++)
		if (buff[i] != data_const_head[i]) {
			ERROR("\n\tlen: %d, i:%d\n\n", length, i);
			return;
		}

	switch (cmd_operate_mode) {
	default:			ERROR("\n\tknob: error: operate_mode = %d\n\n", cmd_operate_mode);
	break;case 0x00:	cmd_operate_mode = buff[16] + 1;
	break;case 0x01:	knob_core_mouse_scroll(buff[16]);
	break;case 0x02:	knob_core_volume(buff[16]);
	break;case 0x03:	knob_core_music(buff[16]);
	break;case 0x04:	knob_core_lamp_light(buff[16]);
	break;case 0x05:	knob_core_screen_light(buff[16]);
	break;case 0x06:	knob_core_tab(buff[16]);
	}

	is_wating = false;

	// LOG("");
}


static void knob_write_cmd(s8 operate_mode) {
	if (operate_mode == -1)
		return;

	u8 cmd[10];

	for (u8 i = 0; i < 5; i++) {
		cmd[i] = cmd_const_head[i];
		cmd[i + 5] = cmd_const_body[operate_mode][i];
	}

	uart_write(UART_BUS_ID(1), cmd, sizeof(cmd));
	is_wating = true;

	while (is_wating) {
		__asm("nop");
	}

	// LOG("");
}


void knob_init(void) {
	uart_set_r_cb(UART_BUS_ID(1), knob_uart_r_int_handler);
}


void knob_oneloop(void) {
	// knob_is_bad = false;
	cmd_operate_mode = 0;

	knob_write_cmd(cmd_operate_mode);

	// if (knob_is_bad == false)
	knob_write_cmd(cmd_operate_mode);

	cmd_operate_mode = -1;
}


void knob_exit(void) {}

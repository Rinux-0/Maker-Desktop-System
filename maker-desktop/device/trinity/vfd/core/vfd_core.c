#include "vfd_core.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"



static u8 cmd[10];
static u8 data[2][20];

static u8 cmd_clear[] = "\x1b[2J";					// 清屏
static u8 cmd_clear_back[] = "\x1b[0K";				// 当前行，清除光标后的内容
static u8 cmd_set_cursor_pos[] = "\x1b[%02u;%02uH";	// 光标定位



void vfd_core_flush_line(u8 line) {
	// 定位(1,line)
	sprintf((c8*)cmd, (c8*)cmd_set_cursor_pos, line, 1);
	uart_write(UART_BUS_ID(2), cmd, sizeof(cmd));

	// 覆盖输出
	uart_write(UART_BUS_ID(2), (u8*)data[line], sizeof(data[line]));
}


void vfd_core_flush_screen(void) {
	for (u8 i = 0; i < 2; i++)
		vfd_core_flush_line(i);
}


// 原点：左上角
void vfd_core_set_char(c8 ch, u8 x, u8 y, bool flush) {
	// 定位(1,line)
	sprintf((c8*)cmd, (c8*)cmd_set_cursor_pos, y, x);
	uart_write(UART_BUS_ID(2), cmd, sizeof(cmd));

	// 覆盖输出
	uart_write(UART_BUS_ID(2), (u8*)data[y], sizeof(data[y]));
}


void vfd_core_set_line(u8* buff_20, u8 line, bool flush) {
	for (u8 i = 0; i < 20; i++) {
		data[line][i] = buff_20[i];
	}

	if (flush) {
		vfd_core_flush_line(line);
	}
}


void vfd_core_set_screen(u8* l1_20, u8* l2_20, bool flush) {
	for (u8 i = 0; i < 20; i++) {
		data[0][i] = l1_20[i];
		data[1][i] = l2_20[i];
	}

	if (flush) {
		vfd_core_flush_screen();
	}
}

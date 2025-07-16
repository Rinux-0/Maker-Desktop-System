#include "vfd_core.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"



static u8 cmd[10];
static u8 data[2][20];

static u8 cmd_clear[] = "\x1b[2J";					// 清屏
static u8 cmd_clear_back[] = "\x1b[0K";				// 当前行，清除光标后的内容
static u8 cmd_set_cursor_pos[] = "\x1b[%02u;%02uH";	// 光标定位

static volatile bool is_wating = false;



void vfd_core_set_pos(u8 x, u8 y) {
	// 定位(x,y)
	sprintf((c8*)cmd, (c8*)cmd_set_cursor_pos, y, x);
	uart_write(UART_BUS_ID(2), cmd, strlen((c8*)cmd));
}


void vfd_core_flush_line(u8 line) {
	// 定位(1,line)
	vfd_core_set_pos(1, line);

	// 覆盖输出
	uart_write(UART_BUS_ID(2), (u8*)data[line - 1], sizeof(data[line - 1]));
}


void vfd_core_flush_screen(void) {
	for (u8 i = 1; i <= 2; i++)
		vfd_core_flush_line(i);
}


// 原点：左上角(1,1)
void vfd_core_set_char(c8 ch, u8 x, u8 y, bool flush) {
	// 定位(x,y)
	vfd_core_set_pos(x, y);

	// 覆盖输出
	uart_write(UART_BUS_ID(2), (u8*)&ch, 1);
}


void vfd_core_set_line(u8* buff_20, u8 line, bool flush) {
	for (u8 i = 0; i < 20; i++) {
		data[line - 1][i] = buff_20[i];
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


static bool vfd_core_process_cmd(c8* sub_cmd, u8 sub_len) {
	c8* tmp;

	// if ((tmp = strnstr(sub_cmd, "sle_conn_status", sub_len))) {		// sle_conn_status
	// 	DATA("sle_conn_status ");
	// 	sub_cmd = sizeof("sle_conn_status") + tmp;
	// 	sub_len -= sizeof("sle_conn_status");


	// } else {
	// 	return false;
	// }

	return true;
}


void vfd_cmd_entry(c8* buff, u16 length) {
	if (is_wating) {
		LOG("\n\tLast vfd cmd is processing ...\n\n");
		return;
	}

	is_wating = true;

	c8* substr = strnstr(buff, "vfd", length);
	if (substr == NULL) {
		// ERROR("\n\tcmd error: [%s]\n\n", buff);
		is_wating = false;
		return;
	}

	DATA("\n\tGet vfd cmd[%u]: [ ", length);
	substr += sizeof("vfd");
	length -= sizeof("vfd");

	if (false == vfd_core_process_cmd(substr, length))
		DATA("ERROR... ]\n\n");

	is_wating = false;
}

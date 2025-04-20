#include "tempreture.h"

#include "uuart.h"
#include "def.h"
#include "tool.h"



void tempreture_get_head_req(void) {
	const u8 data[] = "\xA1\xAB";

	uart_write(2, data, sizeof(data)-1);
	LOG("send head req\n");
}


void tempreture_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	// u8* buff = (u8*)buffer;
	// u8 data = atoi((const char*)buff);

	/// @todo WIFI 发给上位机
	LOG((const char*)buffer);
}

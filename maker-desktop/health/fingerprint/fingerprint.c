#include "fingerprint.h"

#include "ddef.h"
#include "ttool.h"



void fingerprint_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	// u8* buff = (u8*)buffer;
	// u8 data = atoi((const char*)buffer);

	/// @todo WIFI 发给上位机
	// LOG((const char*)data);
}

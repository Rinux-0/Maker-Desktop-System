#include "wwifi.h"

#include "ddef.h"
#include "ttool.h"

#include "udp_client.h"

#include <cmsis_os2.h>



/*static */void wifi__init(void) {
	udp_client_init();
}


/*static */void wifi_oneloop(void) {
	tool_sleep_m(1);

	udp_client_oneloop();
}


/*static */void wifi_exit(void) {
	udp_client_exit();
}


// static void* wifi(const c8* arg) {
// 	unused(arg);

// 	wifi__init();
// 	while (1)
// 		wifi_oneloop();
// 	wifi_exit();

// 	return NULL;
// }


// void wifi_entry(void) {
// 	osThreadAttr_t attr = {
// 		.name = "wifi",
// 		.attr_bits = 0U,
// 		.cb_mem = NULL,
// 		.cb_size = 0U,
// 		.stack_mem = NULL,
// 		.stack_size = 0x1000,
// 		.priority = (osPriority_t)13,
// 		// .tz_module	= 0U,
// 		// .reserved	= 0U
// 	};

// 	if (NULL == osThreadNew((osThreadFunc_t)wifi, NULL, &attr)) {
// 		ERROR("Failed to create wifi sub_thread");
// 	}

// 	LOG("");
// }


void wifi_modify_cfg(u8* wifi_ssid, u8* wifi_pwd, u8* wifi_ip, u16 wifi_port) {
	udp_client_modify_wifi_cfg(wifi_ssid, wifi_pwd, wifi_ip, wifi_port);
}


void wifi_write(const u8* data, u16 len) {
	udp_client_send_data(data, len);
}

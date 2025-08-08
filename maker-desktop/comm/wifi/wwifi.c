#include "wwifi.h"

#include "ddef.h"
#include "ttool.h"

#include "http.h"
#include "wifi_connect.h"
// #include "udp_client.h"

#include <cmsis_os2.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>



static u8 wifi_server_ssid[24] = "Rinux";
static u8 wifi_server_pwd[24] = "87492324";

u8 wifi_server_ip[24] = "192.168.220.49";
static u16 wifi_server_port = 8749;



/*static */void wifi__init(void) {
	// 连接Wifi
	wifi_connect((c8*)wifi_server_ssid, (c8*)wifi_server_pwd);

	// 初始化预连接的服务端地址信息
	// 服务器的地址信息
	static struct sockaddr_in send_addr;
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(wifi_server_port);
	send_addr.sin_addr.s_addr = inet_addr((c8*)wifi_server_ip);

	http_init(
		(struct sockaddr*)&send_addr, sizeof(send_addr),
		wifi_server_ip, wifi_server_port
	);
	// udp_client_init();
}


/*static */void wifi_oneloop(void) {
	tool_sleep_m(1);

	if (false == wifi_is_conn()) {
		wifi__init();
	} else {
		http_oneloop();
		// udp_client_oneloop();
	}
}


/*static */void wifi_exit(void) {
	http_exit();
	// udp_client_exit();
}


static void* wifi(const c8* arg) {
	unused(arg);

	wifi__init();
	while (1)
		wifi_oneloop();
	wifi_exit();

	return NULL;
}


void wifi_entry(void) {
	osThreadAttr_t attr = {
		.name = "wifi",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)13,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)wifi, NULL, &attr)) {
		ERROR("Failed to create wifi sub_thread");
	}

	LOG("");
}


// 不安全
void wifi_modify_cfg(u8* wifi_ssid, u8* wifi_pwd, u8* wifi_ip, s32 wifi_port) {
	if (wifi_ssid)
		strcpy((c8*)wifi_server_ssid, (c8*)wifi_ssid);
	if (wifi_pwd)
		strcpy((c8*)wifi_server_pwd, (c8*)wifi_pwd);
	if (wifi_ip)
		strcpy((c8*)wifi_server_ip, (c8*)wifi_ip);
	if (wifi_port >= 0 && wifi_port < 65536)
		wifi_server_port = wifi_port;

	DATA("\n\n\t[ssid: %s][pwd: %s]\n\t[ip: %s][port: %u]\n\n\n",
		(c8*)wifi_server_ssid, (c8*)wifi_server_pwd,
		(c8*)wifi_server_ip, wifi_server_port
	);

	wifi__init();

	// udp_client_modify_wifi_cfg(wifi_ssid, wifi_pwd, wifi_ip, wifi_port);
}


void wifi_write(c8* request_way, c8* url_path, bool is_conn_keep_alive, c8* data) {
	if (false == wifi_is_conn())
		ERROR("wifi isn't connected");

	http_send(request_way, url_path, is_conn_keep_alive, data);
}
// void wifi_write(const u8* data, u16 len) {
// 	udp_client_send_data(data, len);
// }

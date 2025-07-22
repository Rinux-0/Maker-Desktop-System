#include "ssle_client_core.h"
#include "../ssle_def.h"

#include "bts_le_gap.h"
#include "ddef.h"
#include "product.h"
#include "securec.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_client.h"
#include "soc_osal.h"
#include "ttool.h"
#include "uuart.h"



static u8 willserver_index;
static u8 sle_client_conn_num;
s8 sle_client_conn_id[sle_target_max] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static sle_addr_t					 sle_client_remote_addr;
static sle_announce_seek_callbacks_t sle_client_seek_cbk;
static sle_connection_callbacks_t	 sle_client_connect_cbk;
static ssapc_callbacks_t			 sle_client_ssapc_cbk;
static ssapc_find_service_result_t	 sle_client_find_service_result;
static ssapc_write_param_t			 sle_client_send_param;



s8 sle_client_find_server_index_by_name(const u8* name) {
	if (strstr((c8*)name, "pc")) {
		return 0;
	} else if (strstr((c8*)name, "deskaide")) {
		return 1;
	} else if (strstr((c8*)name, "dynamic")) {
		return 2;
	} else if (strstr((c8*)name, "health")) {
		return 3;
	} else if (strstr((c8*)name, "keyboard")) {
		return 4;
	} else if (strstr((c8*)name, "keypad")) {
		return 5;
	} else if (strstr((c8*)name, "receiver")) {
		return 6;
	} else if (strstr((c8*)name, "keytest")) {
		return 7;
	} else if (strstr((c8*)name, "tmptest")) {
		return 8;
	} else {
		return -1;
	}
}


s8 sle_client_find_server_index_by_conn_id(u16 conn_id) {
	for (u8 i = 0; i < sle_target_max; i++)
		if (conn_id == sle_client_conn_id[i])
			return i;

	return -1;
}


ssapc_write_param_t* sle_client_get_send_param(void) {
	// LOG("");

	return &sle_client_send_param;
}


static void sle_client_start_scan(void) {
	LOG("");

	sle_seek_param_t param = {
		.own_addr_type = 0,
		.filter_duplicates = 0,
		.seek_filter_policy = 0,
		.seek_phys = 1,
		.seek_type[0] = 1,
		.seek_interval[0] = SLE_SEEK_INTERVAL,
		.seek_window[0] = SLE_SEEK_WINDOW,
	};

	sle_set_seek_param(&param);
	sle_start_seek();
}


// sle_server -> 我 -> uart (无应答)
static void sle_client_notification_cb(u8 client_id, u16 conn_id, ssapc_handle_value_t* data, errcode_t status) {
	unused(client_id);
	unused(conn_id);
	unused(status);

	// LOG("\n sle uart recived data : %s\n", data->data);

	uart_write(1, (u8*)(data->data), data->data_len);
}


// sle_server -> 我 -> uart (有应答)
static void sle_client_indication_cb(u8 client_id, u16 conn_id, ssapc_handle_value_t* data, errcode_t status) {
	// LOG("");

	sle_client_notification_cb(client_id, conn_id, data, status);
}


static void sle_client_sle_enable_cbk(errcode_t status) {
	// LOG("status: %d.\n", status);

	sle_client_init_core();
	sle_client_start_scan();
}


static void sle_client_seek_enable_cbk(errcode_t status) {
	// LOG("");

	if (status != 0)
		ERROR("%s status error = %x\n", SLE_CLIENT_LOG, status);
}


static void sle_client_seek_result_info_cbk(sle_seek_result_info_t* seek_result_data) {
	if (strstr((const c8*)seek_result_data->data, SLE_COMM_SHIT))
		return;

	// LOG("seek_result_data:\n\taddr:[%02x,%02x,%02x,%02x,%02x,%02x]\n",
	// 	seek_result_data->addr.addr[0], seek_result_data->addr.addr[1], seek_result_data->addr.addr[2], seek_result_data->addr.addr[3], seek_result_data->addr.addr[4], seek_result_data->addr.addr[5]
	// ); DATA("\tdata:%s\n", seek_result_data->data);

	if (seek_result_data != NULL)
		if (sle_client_conn_num < SLE_CLIENT_MAX_CONN) {
			u8* substr = (u8*)strstr((const c8*)seek_result_data->data, SLE_CONN_IDENTITY);
			if (substr != NULL) {
				LOG("will connect dev\n");

				(void)memcpy_s(&sle_client_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));

				willserver_index = sle_client_find_server_index_by_name(substr);

				sle_stop_seek();
			}
		}
}


static void sle_client_seek_disable_cbk(errcode_t status) {
	if (status != 0) {
		ERROR("%s status error = %x\n", SLE_CLIENT_LOG, status);
		return;
	}

	// LOG("");

	sle_connect_remote_device(&sle_client_remote_addr);
}


static void sle_client_register_seek_cbk(void) {
	// LOG("");

	sle_client_seek_cbk.sle_enable_cb = sle_client_sle_enable_cbk;
	sle_client_seek_cbk.seek_enable_cb = sle_client_seek_enable_cbk;
	sle_client_seek_cbk.seek_result_cb = sle_client_seek_result_info_cbk;
	sle_client_seek_cbk.seek_disable_cb = sle_client_seek_disable_cbk;

	sle_announce_seek_register_callbacks(&sle_client_seek_cbk);
}


static void sle_client_connect_state_changed_cbk(u16 conn_id, const sle_addr_t* addr, sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason) {
	unused(addr);
	unused(pair_state);

	LOG("%s disc_reason:0x%x\n", SLE_CLIENT_LOG, disc_reason);

	if (conn_state == SLE_ACB_STATE_CONNECTED) {
		LOG("\n\n\n\t%s SLE_ACB_STATE_CONNECTED\n\n\n", SLE_CLIENT_LOG);

		if (willserver_index != 0) {
			sle_client_conn_id[willserver_index] = conn_id;
			willserver_index = 0;
		}

		ssap_exchange_info_t info = {
			.mtu_size = SLE_MTU_SIZE,
			.version = 1,
		};

		ssapc_exchange_info_req(1, conn_id, &info);

		sle_client_conn_num++;
	} else if (conn_state == SLE_ACB_STATE_NONE) {
		LOG("\n\n\n\t%s SLE_ACB_STATE_NONE\n\n\n", SLE_CLIENT_LOG);

		tool_pin_gpio_set_val(LED_PIN_SLE, 1);
	} else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
		LOG("\n\n\n\t%s SLE_ACB_STATE_DISCONNECTED\n\n\n", SLE_CLIENT_LOG);

		tool_pin_gpio_set_val(LED_PIN_SLE, 0);

		s8 id_tmp = sle_client_find_server_index_by_conn_id(conn_id);
		if (id_tmp != -1)
			sle_client_conn_id[id_tmp] = -1;

		sle_client_conn_num--;

		sle_client_start_scan();
	} else {
		ERROR("%s conn_state error = %x\n", SLE_CLIENT_LOG, conn_state);
	}

	sle_start_seek();
}


static void sle_client_pair_complete_cbk(u16 conn_id, const sle_addr_t* addr, errcode_t status) {
	LOG("%s conn_id:%d, addr:%02x***%02x%02x\n",
		SLE_CLIENT_LOG, conn_id, addr->addr[0], addr->addr[4], addr->addr[5]
	);

	if (status != 0) {
		ERROR("%s status error = %x\n", SLE_CLIENT_LOG, status);
		return;
	}

	ssap_exchange_info_t info = {
		.mtu_size = SLE_MTU_SIZE,
		.version = 1,
	};

	ssapc_exchange_info_req(0, sle_client_conn_id[willserver_index], &info);
}


static void sle_client_register_connect_cbk(void) {
	// LOG("");

	sle_client_connect_cbk.connect_state_changed_cb = sle_client_connect_state_changed_cbk;
	sle_client_connect_cbk.pair_complete_cb = sle_client_pair_complete_cbk;

	sle_connection_register_callbacks(&sle_client_connect_cbk);
}


static void sle_client_exchange_info_cbk(u8 client_id, u16 conn_id, ssap_exchange_info_t* param, errcode_t status) {
	// LOG("%s pair complete client id:%d status:%d\n",
	// 	SLE_CLIENT_LOG, client_id, status
	// ); LOG("%s exchange mtu, mtu size: %d, version: %d.\n",
	// 	SLE_CLIENT_LOG, param->mtu_size, param->version
	// );

	ssapc_find_structure_param_t find_param = {
		.type = SSAP_FIND_TYPE_PROPERTY,
		.start_hdl = 1,
		.end_hdl = 0xFFFF,
	};

	ssapc_find_structure(0, conn_id, &find_param);
}


static void sle_client_find_structure_cbk(u8 client_id, u16 conn_id, ssapc_find_service_result_t* service, errcode_t status) {
	// LOG("%s client: %d conn_id:%d status: %d \n",
	// 	SLE_CLIENT_LOG, client_id, conn_id, status
	// ); LOG("%s start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\n",
	// 	SLE_CLIENT_LOG, service->start_hdl, service->end_hdl, service->uuid.len
	// );

	sle_client_find_service_result.start_hdl = service->start_hdl;
	sle_client_find_service_result.end_hdl = service->end_hdl;

	(void)memcpy_s(&sle_client_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}


static void sle_client_find_property_cbk(u8 client_id, u16 conn_id, ssapc_find_property_result_t* property, errcode_t status) {
	// LOG("%s client id: %d, conn id: %d, operate ind: %d, "
	// 	"descriptors count: %d status:%d property->handle %d\n",
	// 	SLE_CLIENT_LOG, client_id, conn_id, property->operate_indication, property->descriptors_count, status, property->handle
	// );

	sle_client_send_param.handle = property->handle;
	sle_client_send_param.type = SSAP_PROPERTY_TYPE_VALUE;
}


static void sle_client_find_structure_cmp_cbk(u8 client_id, u16 conn_id, ssapc_find_structure_result_t* structure_result, errcode_t status) {
	unused(conn_id);

	// LOG("%s client id:%d status:%d type:%d uuid len:%d \n",
	// 	SLE_CLIENT_LOG, client_id, status, structure_result->type, structure_result->uuid.len
	// );
}


static void sle_client_write_cfm_cb(u8 client_id, u16 conn_id, ssapc_write_result_t* write_result, errcode_t status) {
	// LOG("%s conn_id:%d client id:%d status:%d handle:%02x type:%02x\n",
	// 	SLE_CLIENT_LOG, conn_id, client_id, status, write_result->handle, write_result->type
	// );
}


static void sle_client_register_ssapc_cbk(void) {
	// LOG("");

	sle_client_ssapc_cbk.exchange_info_cb = sle_client_exchange_info_cbk;
	sle_client_ssapc_cbk.find_structure_cb = sle_client_find_structure_cbk;
	sle_client_ssapc_cbk.ssapc_find_property_cbk = sle_client_find_property_cbk;
	sle_client_ssapc_cbk.find_structure_cmp_cb = sle_client_find_structure_cmp_cbk;
	sle_client_ssapc_cbk.write_cfm_cb = sle_client_write_cfm_cb;
	sle_client_ssapc_cbk.notification_cb = sle_client_notification_cb;
	sle_client_ssapc_cbk.indication_cb = sle_client_indication_cb;

	ssapc_register_callbacks(&sle_client_ssapc_cbk);
}


errcode_t sle_client_set_r_cb(sle_r_cb_t r_cb) {
	// LOG("");

	sle_client_ssapc_cbk.notification_cb = (ssapc_notification_callback)r_cb;
	sle_client_ssapc_cbk.indication_cb = (ssapc_indication_callback)r_cb;

	return ssapc_register_callbacks(&sle_client_ssapc_cbk);
}


void sle_client_init_core(void) {
	LOG("");

	sle_client_register_seek_cbk();
	sle_client_register_connect_cbk();
	sle_client_register_ssapc_cbk();
}

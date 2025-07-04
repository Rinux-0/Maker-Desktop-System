#include "ssle_server_core.h"
#include "../ssle_def.h"

#include "ddef.h"
#include "ttool.h"

#include "uuart.h"

#include "ssle_server_adv.h"

#include <securec.h>
#include <sle_connection_manager.h>
#include <sle_device_discovery.h>
#include <sle_errcode.h>
#include <sle_ssap_server.h>
#include <soc_osal.h>



// typedef struct {
// 	u8 target_id;
// 	u8* data;
// 	u32 length;
// } sle_server_msg_t;



static u8 g_sle_uuid_app_uuid[SLE_UUID_LEN_2] = { 0x12, 0x34 };							// sle server app uuid for test
static u8 g_sle_property_value[SLE_OCTET_BIT_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };	// server notify property uuid for test
static u16 g_sle_conn_hdl = 0;		// sle conn acb handle
static u8  g_server_id = 0;			// sle server handle
static u16 g_service_hdl = 0;		// sle service handle
static u16 g_property_hdl = 0;		// sle ntf property handle
static u16 g_sle_pair_hdl = 0;		// sle pair acb handle
static sle_server_msg_queue g_sle_server_msg_queue = NULL;
static u8 g_sle_base[] = {
	0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
	0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



static void sle_server_init_led(void) {
	uapi_pin_set_mode(LED_PIN_SLE, PIN_MODE_0);
	uapi_gpio_set_dir(LED_PIN_SLE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN_SLE, GPIO_LEVEL_HIGH);
}


static void sle_server_ctrl_led(bool on) {
	LOG("");

	tool_pin_gpio_set_val(LED_PIN_SLE, !on);
}


u16 sle_server_get_conn_id(void) {
	LOG("");

	return g_sle_conn_hdl;
}


static void sle_server_encode2byte_little(u8* _ptr, u16 data) {
	LOG("");

	*(u8*)((_ptr)+1) = (u8)((data) >> 0x8);
	*(u8*)(_ptr) = (u8)(data);
}


static void sle_server_uuid_set_base(sle_uuid_t* out) {
	if (EOK != memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_base, SLE_UUID_LEN)) {
		ERROR("%s memcpy fail\n", SLE_SERVER_LOG);
		out->len = 0;
		return;
	}

	LOG("");

	out->len = SLE_UUID_LEN_2;
}


static void sle_server_uuid_setu2(u16 u2, sle_uuid_t* out) {
	LOG("");

	sle_server_uuid_set_base(out);
	sle_server_encode2byte_little(&out->uuid[SLE_UUID_INDEX], u2);
}


static void sle_server_uuid_print(sle_uuid_t* uuid) {
	if (uuid == NULL) {
		ERROR("%s uuid is null\n", SLE_SERVER_LOG);
		return;
	}

	LOG("");

	if (uuid->len == SLE_UUID_16BIT_LEN) {
		DATA("%s uuid: %02x %02x.\n", SLE_SERVER_LOG, uuid->uuid[14], uuid->uuid[15]); /* 14 15: uuid index */
	} else if (uuid->len == SLE_UUID_128BIT_LEN) {
		DATA("%s uuid: \n", SLE_SERVER_LOG); /* 14 15: uuid index */
		DATA("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[0], uuid->uuid[1], uuid->uuid[2], uuid->uuid[3]);
		DATA("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[4], uuid->uuid[5], uuid->uuid[6], uuid->uuid[7]);
		DATA("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[8], uuid->uuid[9], uuid->uuid[10], uuid->uuid[11]);
		DATA("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[12], uuid->uuid[13], uuid->uuid[14], uuid->uuid[15]);
	}
}


static void sle_server_ssaps_mtu_changed_cbk(u8 server_id, u16 conn_id, ssap_exchange_info_t* mtu_size, errcode_t status) {
	LOG("%s ssaps server_id:%x, conn_id:%x, mtu_size:%x, status:%x\n", SLE_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);

	if (g_sle_pair_hdl == 0)
		g_sle_pair_hdl = conn_id + 1;
}


static void sle_server_ssaps_start_service_cbk(u8 server_id, u16 handle, errcode_t status) {
	LOG("%s server_id:%d, handle:%x, status:%x\n", SLE_SERVER_LOG, server_id, handle, status);
}


static void sle_server_ssaps_add_service_cbk(u8 server_id, sle_uuid_t* uuid, u16 handle, errcode_t status) {
	LOG("%s server_id:%x, handle:%x, status:%x\n", SLE_SERVER_LOG, server_id, handle, status);

	sle_server_uuid_print(uuid);
}


static void sle_server_ssaps_add_property_cbk(u8 server_id, sle_uuid_t* uuid, u16 service_handle, u16 handle, errcode_t status) {
	LOG("%s server_id:%x, service_handle:%x,handle:%x, status:%x\n", SLE_SERVER_LOG, server_id, service_handle, handle, status);

	sle_server_uuid_print(uuid);
}


static void sle_server_ssaps_add_descriptor_cbk(u8 server_id, sle_uuid_t* uuid, u16 service_handle, u16 property_handle, errcode_t status) {
	LOG("%s server_id:%x, service_handle:%x, property_handle:%x, status:%x\n", SLE_SERVER_LOG, server_id, service_handle, property_handle, status);

	sle_server_uuid_print(uuid);
}


static void sle_server_ssaps_delete_all_service_cbk(u8 server_id, errcode_t status) {
	LOG("%s server_id:%x, status:%x\n", SLE_SERVER_LOG, server_id, status);
}


static errcode_t sle_server_ssaps_register_cbks(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback ssaps_write_callback) {
	ssaps_callbacks_t ssaps_cbk = {
		.add_service_cb = sle_server_ssaps_add_service_cbk,
		.add_property_cb = sle_server_ssaps_add_property_cbk,
		.add_descriptor_cb = sle_server_ssaps_add_descriptor_cbk,
		.start_service_cb = sle_server_ssaps_start_service_cbk,
		.delete_all_service_cb = sle_server_ssaps_delete_all_service_cbk,
		.mtu_changed_cb = sle_server_ssaps_mtu_changed_cbk,
		.read_request_cb = ssaps_read_callback,				// read：发给 client , 让 client 读 ！！！
		.write_request_cb = ssaps_write_callback			// write：收到 client 的写， server 自己读 ！！！
	};

	errcode_t ret = ssaps_register_callbacks(&ssaps_cbk);

	LOG("");

	if (ret != ERRCODE_SLE_SUCCESS)
		ERROR("%s ssaps_register_callbacks fail :%x\n", SLE_SERVER_LOG, ret);

	return ret;
}


static errcode_t sle_server_uuid_add_service(void) {
	sle_uuid_t service_uuid = { 0 };

	sle_server_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);

	errcode_t ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_hdl);

	LOG("");

	if (ret != ERRCODE_SLE_SUCCESS)
		ERROR("%s fail, ret:%x\n", SLE_SERVER_LOG, ret);

	return ret;
}


static errcode_t sle_server_uuid_add_property(void) {
	u8 ntf_value[] = { 0x01, 0x0 };
	ssaps_property_info_t property = {
		.permissions = SLE_UUID_TEST_PROPERTIES,
		.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
		.value = (u8*)osal_vmalloc(sizeof(g_sle_property_value)),
	};
	ssaps_desc_info_t descriptor = {
		.permissions = SLE_UUID_TEST_DESCRIPTOR,
		.type = SSAP_DESCRIPTOR_USER_DESCRIPTION,
		.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE,
		.value = ntf_value,
		.value_len = sizeof(ntf_value),
	};
	errcode_t ret = 0;

	LOG("");

	if (property.value == NULL) {
		osal_vfree(property.value);
		return ERRCODE_SLE_FAIL;
	}

	sle_server_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);

	if (EOK != memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value, sizeof(g_sle_property_value))) {
		osal_vfree(property.value);
		return ERRCODE_SLE_FAIL;
	}

	ret = ssaps_add_property_sync(g_server_id, g_service_hdl, &property, &g_property_hdl);
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s sle add property fail, ret:%x\n", SLE_SERVER_LOG, ret);
		osal_vfree(property.value);
		return ret;
	}

	ret = ssaps_add_descriptor_sync(g_server_id, g_service_hdl, g_property_hdl, &descriptor);
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s sle add descriptor fail, ret:%x\n", SLE_SERVER_LOG, ret);
		osal_vfree(property.value);
		return ret;
	}

	osal_vfree(property.value);

	return ERRCODE_SLE_SUCCESS;
}


static errcode_t sle_server_add(void) {
	errcode_t ret = 0;
	sle_uuid_t app_uuid = { 0 };

	LOG("%s func in\n", SLE_SERVER_LOG);

	app_uuid.len = sizeof(g_sle_uuid_app_uuid);
	if (EOK != memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)))
		return ERRCODE_SLE_FAIL;
	ssaps_register_server(&app_uuid, &g_server_id);

	ret = sle_server_uuid_add_service();
	if (ret != ERRCODE_SLE_SUCCESS) {
		ssaps_unregister_server(g_server_id);
		return ret;
	}

	ret = sle_server_uuid_add_property();
	if (ret != ERRCODE_SLE_SUCCESS) {
		ssaps_unregister_server(g_server_id);
		return ret;
	}

	LOG("%s server_id:%x, service_handle:%x, property_handle:%x\n", SLE_SERVER_LOG, g_server_id, g_service_hdl, g_property_hdl);

	ret = ssaps_start_service(g_server_id, g_service_hdl);
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s fail, ret:%x\n", SLE_SERVER_LOG, ret);
		return ret;
	}

	LOG("%s func out\n", SLE_SERVER_LOG);

	return ret;
}


/* device 通过 uuid 向 host 发送数据：report */
errcode_t sle_server_send_report_by_uuid(sle_target_t target, const u8* data, u8 len) {
	ssaps_ntf_ind_by_uuid_t param = {
		.type = SSAP_PROPERTY_TYPE_VALUE,
		.start_handle = g_service_hdl,
		.end_handle = g_property_hdl,
		.value_len = len + 2,
		.value = (u8*)osal_vmalloc(len + 2)
	};

	if (param.value == NULL) {
		ERROR("%s send report new fail\n", SLE_SERVER_LOG);
		return ERRCODE_SLE_FAIL;
	}

	LOG("");

	param.value[0] = '@';
	param.value[1] = target;

	if (EOK != memcpy_s(param.value + 2, param.value_len - 2, data, len)) {
		ERROR("%s send input report memcpy fail\n", SLE_SERVER_LOG);
		osal_vfree(param.value);
		return ERRCODE_SLE_FAIL;
	}

	sle_server_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);

	errcode_t ret = ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_hdl, &param);

	if (ret != ERRCODE_SLE_SUCCESS)
		ERROR("%s ssaps_notify_indicate_by_uuid fail :%x\n", SLE_SERVER_LOG, ret);

	osal_vfree(param.value);

	return ret;
}


/* device通过handle向host发送数据：report */
errcode_t sle_server_send_report_by_hdl(sle_target_t target, const u8* data, u16 len) {
	static u8 receive_buf[SLE_UART_BUFF_LENGTH] = { 0 }; /* max receive length. */

	ssaps_ntf_ind_t param = {
		.handle = g_property_hdl,
		.type = SSAP_PROPERTY_TYPE_VALUE,
		.value_len = len + 2,
		.value = receive_buf
	};

	// LOG("");

	param.value[0] = '@';
	param.value[1] = target;

	if (EOK != memcpy_s(param.value + 2, param.value_len - 2, data, len)) {
		ERROR("%s send input report memcpy fail\n", SLE_SERVER_LOG);
		return ERRCODE_SLE_FAIL;
	}

	return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}


static void sle_server_conn_state_changed_cbk(u16 conn_id, const sle_addr_t* addr, sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason) {
	u8 sle_conn_state[] = "sle_dis_connect";

	LOG("%s conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\n",
		SLE_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason
	); DATA("addr:%02x:**:**:**:%02x:%02x\n",
		addr->addr[SLE_BT_INDEX_0], addr->addr[SLE_BT_INDEX_4]
	);

	if (conn_state == SLE_ACB_STATE_CONNECTED) {
		sle_server_ctrl_led(true);
		g_sle_conn_hdl = conn_id;
		LOG("\n\n\n\t%s SLE_ACB_STATE_CONNECTED\n\n\n", SLE_SERVER_LOG);
	} else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
		sle_server_ctrl_led(false);
		g_sle_conn_hdl = g_sle_pair_hdl = 0;
		if (g_sle_server_msg_queue != NULL)
			g_sle_server_msg_queue(sle_conn_state, sizeof(sle_conn_state));
		LOG("\n\n\n\t%s SLE_ACB_STATE_DISCONNECTED\n\n\n", SLE_SERVER_LOG);
	}
}


static void sle_server_pair_complete_cbk(u16 conn_id, const sle_addr_t* addr, errcode_t status) {
	ssap_exchange_info_t parameter = {
		.mtu_size = 520,
		.version = 1,
	};

	LOG("%s pair complete conn_id:%02x, status:%x\n", SLE_SERVER_LOG, conn_id, status);
	DATA("%s pair complete addr:%02x:**:**:**:%02x:%02x\n",
		SLE_SERVER_LOG, addr->addr[SLE_BT_INDEX_0], addr->addr[SLE_BT_INDEX_4]
	);

	g_sle_pair_hdl = conn_id + 1;

	ssaps_set_info(g_server_id, &parameter);
}


static errcode_t sle_server_conn_register_cbks(void) {
	sle_connection_callbacks_t conn_cbks = {
		.connect_state_changed_cb = sle_server_conn_state_changed_cbk,
		.pair_complete_cb = sle_server_pair_complete_cbk,
	};

	errcode_t ret = sle_connection_register_callbacks(&conn_cbks);

	LOG("");

	if (ret != ERRCODE_SLE_SUCCESS)
		ERROR("%s sle_connection_register_callbacks fail :%x\n", SLE_SERVER_LOG, ret);

	return ret;
}


u16 sle_server_is_client_connected(void) {
	LOG("");

	return g_sle_pair_hdl;
}


static void sle_server_ssaps_real_w_request_cbk(u8 server_id, u16 conn_id, ssaps_req_read_cb_t* read_cb_para, errcode_t status) {
	LOG("%s server_id:%x, conn_id:%x, handle:%x, status:%x\n",
		SLE_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status
	);
}


static void sle_server_ssaps_real_r_request_cbk(u8 server_id, u16 conn_id, ssaps_req_write_cb_t* write_cb_para, errcode_t status) {
	LOG("%s server_id:%x, conn_id:%x, handle:%x, status:%x\n",
		SLE_SERVER_LOG, server_id, conn_id, write_cb_para->handle, status
	);

	if (write_cb_para->length > 0 && write_cb_para->value) {
		DATA("\n sle recived data : %s\n", write_cb_para->value);
		uart_write(UART_BUS_ID(0), (u8*)write_cb_para->value, write_cb_para->length);
	}
}


void sle_server_register_msg(sle_server_msg_queue sle_server_msg) {
	LOG("");

	g_sle_server_msg_queue = sle_server_msg;
}


errcode_t sle_server_set_r_cb(sle_r_cb_t r_cb) {
	LOG("");

	// return sle_server_ssaps_register_cbks((ssaps_read_request_callback)r_cb, sle_server_ssaps_w_request_cbk);
	return sle_server_ssaps_register_cbks(
		sle_server_ssaps_real_w_request_cbk,
		r_cb
	);
}


errcode_t sle_server_init_core(void) {
	sle_server_init_led();

	errcode_t ret = sle_server_conn_register_cbks();
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s sle_conn_register_cbks fail :%x\n", SLE_SERVER_LOG, ret);
		return ret;
	}

	ret = sle_server_set_r_cb((sle_r_cb_t)sle_server_ssaps_real_r_request_cbk);
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s sle_ssaps_register_cbks fail :%x\n", SLE_SERVER_LOG, ret);
		return ret;
	}

	ret = sle_server_add();
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s sle_server_add fail :%x\n", SLE_SERVER_LOG, ret);
		return ret;
	}

	LOG("%s init ok\n", SLE_SERVER_LOG);

	return ERRCODE_SLE_SUCCESS;
}

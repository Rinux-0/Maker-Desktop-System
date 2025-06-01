#include "ssle_server_adv.h"
#include "../ssle_def.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle_server_core.h"

#include "demo_def.h"
#include MY_HEADER_NAME(DEV_OR_TEST, h)

#include <osal_addr.h>
#include <osal_debug.h>
#include <osal_task.h>
#include <product.h>
#include <securec.h>
#include <sle_common.h>
#include <sle_device_discovery.h>
#include <sle_errcode.h>



static u8 sle_server_adv_addr[SLE_ADDR_LEN] = { 0x01, 0x02, 0x03, 0x04, 0x01, 0x01 };
static u8 sle_server_adv_name[] = (SLE_CONN_IDENTITY   " ["   STR(CONCAT2(DEV_OR_TEST, _NAME))   "]");
static u8 sle_server_adv_name_len = sizeof(sle_server_adv_name) - 1;



static u16 sle_server_adv_set_adv_server_name(u8* adv_data, u16 max_len) {
	u8 idx = 0;

	LOG("%s server_name_len = %d\n", SLE_SERVER_LOG, sle_server_adv_name_len);
	DATA("%s server_name: ", SLE_SERVER_LOG);
	for (u8 i = 0; i < sle_server_adv_name_len; i++)
		DATA("0x%02x ", sle_server_adv_name[i]);
	DATA("\n");

	adv_data[idx++] = sle_server_adv_name_len + 1;
	adv_data[idx++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;

	if (EOK != memcpy_s(&adv_data[idx], max_len - idx, sle_server_adv_name, sle_server_adv_name_len)) {
		ERROR("%s memcpy fail\n", SLE_SERVER_LOG);
		return 0;
	}

	return (u16)idx + sle_server_adv_name_len;
}


static errcode_t sle_server_adv_set_announce_param(void) {
	sle_announce_param_t param = {
		.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE,
		.announce_handle = SLE_ADV_HANDLE,
		.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO,
		.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL,
		.announce_channel_map = SLE_ADV_CHANNEL_MAP,
		.announce_interval_min = SLE_ADV_INTERVAL_MIN,
		.announce_interval_max = SLE_ADV_INTERVAL_MAX,
		.conn_interval_min = SLE_CONN_INTV_MIN,
		.conn_interval_max = SLE_CONN_INTV_MAX,
		.conn_max_latency = SLE_CONN_MAX_LATENCY,
		.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT,
		.announce_tx_power = 18,
	};
	param.own_addr.type = 0;

	if (EOK != memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, sle_server_adv_addr, SLE_ADDR_LEN)) {
		ERROR("%s data memcpy fail\n", SLE_SERVER_LOG);
		return 0;
	}

	LOG("%s sle_local addr: ", SLE_SERVER_LOG);
	for (u8 idx = 0; idx < SLE_ADDR_LEN; idx++)
		DATA("0x%02x ", param.own_addr.addr[idx]);
	DATA("\n");

	return sle_set_announce_param(param.announce_handle, &param);
}


static u16 sle_server_adv_set_adv_data(u8* adv_data) {
	const size_t len = sizeof(le_adv_common_t);
	le_adv_common_t adv_disc_level = {
		.length = len - 1,
		.type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
		.value = SLE_ANNOUNCE_LEVEL_NORMAL,
	};
	le_adv_common_t adv_access_mode = {
		.length = len - 1,
		.type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
		.value = 0,
	};
	u16 idx = 0;

	if (EOK != memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_disc_level, len)) {
		ERROR("%s adv_disc_level memcpy fail\n", SLE_SERVER_LOG);
		return 0;
	}
	idx += len;

	if (EOK != memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_access_mode, len)) {
		ERROR("%s adv_access_mode memcpy fail\n", SLE_SERVER_LOG);
		return 0;
	}
	idx += len;

	LOG("");

	return idx;
}


static u16 sle_server_adv_set_scan_response_data(u8* scan_rsp_data) {
	size_t scan_rsp_data_len = sizeof(le_adv_common_t);
	le_adv_common_t tx_power_level = {
		.length = scan_rsp_data_len - 1,
		.type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
		.value = SLE_ADV_TX_POWER,
	};
	u16 idx = 0;

	if (EOK != memcpy_s(scan_rsp_data, SLE_ADV_DATA_LEN_MAX, &tx_power_level, scan_rsp_data_len)) {
		ERROR("%s memcpy fail\n", SLE_SERVER_LOG);
		return 0;
	}
	idx += scan_rsp_data_len;

	idx += sle_server_adv_set_adv_server_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN_MAX - idx);

	LOG("");

	return idx;
}


static errcode_sle_t sle_server_adv_set_announce_data(void) {
	u8 announce_data[SLE_ADV_DATA_LEN_MAX] = { 0 };
	u8 seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = { 0 };

	sle_announce_data_t data = {
		.announce_data = announce_data,
		.seek_rsp_data = seek_rsp_data,
		.announce_data_len = sle_server_adv_set_adv_data(announce_data),
		.seek_rsp_data_len = sle_server_adv_set_scan_response_data(seek_rsp_data),
	};
	u8 adv_handle = SLE_ADV_HANDLE;

	LOG("%s data.announce_data_len = %d\n", SLE_SERVER_LOG, data.announce_data_len);
	DATA("%s data.announce_data: ", SLE_SERVER_LOG);
	for (u8 data_index = 0; data_index < data.announce_data_len; data_index++)
		DATA("0x%02x ", data.announce_data[data_index]);
	DATA("\n");

	DATA("%s data.seek_rsp_data_len = %d\n", SLE_SERVER_LOG, data.seek_rsp_data_len);
	DATA("%s data.seek_rsp_data: ", SLE_SERVER_LOG);
	for (u8 data_index = 0; data_index < data.seek_rsp_data_len; data_index++)
		DATA("0x%02x ", data.seek_rsp_data[data_index]);
	DATA("\n");

	errcode_sle_t ret = sle_set_announce_data(adv_handle, &data);

	if (ret != ERRCODE_SLE_SUCCESS)
		ERROR("%s set adv param fail.\n", SLE_SERVER_LOG);

	return ret;
}


static void sle_server_adv_announce_enable_cbk(u32 announce_id, errcode_t status) {
	LOG("%s id:%02x, state:%x\n", SLE_SERVER_LOG, announce_id, status);
}


static void sle_server_adv_announce_disable_cbk(u32 announce_id, errcode_t status) {
	LOG("%s id:%02x, state:%x\n", SLE_SERVER_LOG, announce_id, status);
}


static void sle_server_adv_announce_terminal_cbk(u32 announce_id) {
	LOG("%s id:%02x\n", SLE_SERVER_LOG, announce_id);
}


errcode_t sle_server_adv_announce_register_cbks(void) {
	sle_announce_seek_callbacks_t seek_cbks = {
		.announce_enable_cb = sle_server_adv_announce_enable_cbk,
		.announce_disable_cb = sle_server_adv_announce_disable_cbk,
		.announce_terminal_cb = sle_server_adv_announce_terminal_cbk,
	};

	errcode_t ret = sle_announce_seek_register_callbacks(&seek_cbks);

	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s register_callbacks fail :%x\n", SLE_SERVER_LOG, ret);
	} else {
		LOG("");
	}

	return ret;
}


errcode_t sle_server_adv_init(void) {
	errcode_t ret;

	LOG("");

	ret = sle_server_adv_announce_register_cbks();
	if (ret != ERRCODE_SLE_SUCCESS) {
		ERROR("%s sle_server_adv_announce_register_cbks fail :%x\n", SLE_SERVER_LOG, ret);
		return ret;
	}

	sle_server_adv_set_announce_param();
	sle_server_adv_set_announce_data();

	ret = sle_start_announce(SLE_ADV_HANDLE);		// 删掉后不工作，原因未知

	if (ret != ERRCODE_SLE_SUCCESS)
		ERROR("%s sle_start_announce fail :%x\n", SLE_SERVER_LOG, ret);

	return ret;
}

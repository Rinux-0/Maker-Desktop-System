#include "ssle_core_server.h"
#include "ssle_core_server_adv.h"
#include "../ssle_def.h"

#include "def.h"
#include "errcode.h"
#include "securec.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "soc_osal.h"



static char g_sle_uuid_app_uuid[UUID_LEN_2] = { 0x12, 0x34 };                           /* sle server app uuid for test */
static char g_sle_property_value[OCTET_BIT_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };     /* server notify property uuid for test */
static u16 g_sle_conn_hdl = 0;                                                          /* sle connect acb handle */
static u8 g_server_id = 0;                                                              /* sle server handle */
static u16 g_service_handle = 0;                                                        /* sle service handle */
static u16 g_property_handle = 0;                                                       /* sle ntf property handle */
u16 g_sle_pair_hdl;                                                                     /* sle pair acb handle */
static sle_server_msg_queue g_sle_server_msg_queue = NULL;
static u8 g_sle_base[] = {
    0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



u16 get_connect_id(void) {
    return g_sle_conn_hdl;
}


static void encode2byte_little(u8* _ptr, u16 data) {
    *(u8*)((_ptr)+1) = (u8)((data) >> 0x8);
    *(u8*)(_ptr) = (u8)(data);
}


static void sle_uuid_set_base(sle_uuid_t* out) {
    errcode_t ret;
    ret = memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_base, SLE_UUID_LEN);
    if (ret != EOK) {
        osal_printk("%s sle_uuid_set_base memcpy fail\n", SLE_SERVER_LOG);
        out->len = 0;
        return;
    }
    out->len = UUID_LEN_2;
}


static void sle_uuid_setu2(u16 u2, sle_uuid_t* out) {
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}


static void sle_uuid_print(sle_uuid_t* uuid) {
    if (uuid == NULL) {
        osal_printk("%s uuid_print,uuid is null\r\n", SLE_SERVER_LOG);
        return;
    }
    if (uuid->len == UUID_16BIT_LEN) {
        osal_printk("%s uuid: %02x %02x.\n", SLE_SERVER_LOG, uuid->uuid[14], uuid->uuid[15]); /* 14 15: uuid index */
    } else if (uuid->len == UUID_128BIT_LEN) {
        osal_printk("%s uuid: \n", SLE_SERVER_LOG); /* 14 15: uuid index */
        osal_printk("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[0], uuid->uuid[1], uuid->uuid[2], uuid->uuid[3]);
        osal_printk("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[4], uuid->uuid[5], uuid->uuid[6], uuid->uuid[7]);
        osal_printk("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[8], uuid->uuid[9], uuid->uuid[10], uuid->uuid[11]);
        osal_printk("%s 0x%02x 0x%02x 0x%02x \n", SLE_SERVER_LOG, uuid->uuid[12], uuid->uuid[13], uuid->uuid[14], uuid->uuid[15]);
    }
}


static void ssaps_mtu_changed_cbk(u8 server_id, u16 conn_id, ssap_exchange_info_t* mtu_size,
    errcode_t status) {
    osal_printk("%s ssaps ssaps_mtu_changed_cbk callback server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
    if (g_sle_pair_hdl == 0) {
        g_sle_pair_hdl = conn_id + 1;
    }
}


static void ssaps_start_service_cbk(u8 server_id, u16 handle, errcode_t status) {
    osal_printk("%s start service cbk callback server_id:%d, handle:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, handle, status);
}


static void ssaps_add_service_cbk(u8 server_id, sle_uuid_t* uuid, u16 handle, errcode_t status) {
    osal_printk("%s add service cbk callback server_id:%x, handle:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, handle, status);
    sle_uuid_print(uuid);
}


static void ssaps_add_property_cbk(u8 server_id, sle_uuid_t* uuid, u16 service_handle,
    u16 handle, errcode_t status) {
    osal_printk("%s add property cbk callback server_id:%x, service_handle:%x,handle:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, service_handle, handle, status);
    sle_uuid_print(uuid);
}


static void ssaps_add_descriptor_cbk(u8 server_id, sle_uuid_t* uuid, u16 service_handle,
    u16 property_handle, errcode_t status) {
    osal_printk("%s add descriptor cbk callback server_id:%x, service_handle:%x, property_handle:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, service_handle, property_handle, status);
    sle_uuid_print(uuid);
}


static void ssaps_delete_all_service_cbk(u8 server_id, errcode_t status) {
    osal_printk("%s delete all service callback server_id:%x, status:%x\r\n", SLE_SERVER_LOG, server_id, status);
}


static errcode_t sle_ssaps_register_cbks(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback
    ssaps_write_callback) {
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = { 0 };
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_callback;
    ssaps_cbk.write_request_cb = ssaps_write_callback;
    ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_ssaps_register_cbks,ssaps_register_callbacks fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}


static errcode_t sle_uuid_server_service_add(void) {
    errcode_t ret;
    sle_uuid_t service_uuid = { 0 };
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle uuid add service fail, ret:%x\r\n", SLE_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}


static errcode_t sle_uuid_server_property_add(void) {
    errcode_t ret;
    ssaps_property_info_t property = { 0 };
    ssaps_desc_info_t descriptor = { 0 };
    u8 ntf_value[] = { 0x01, 0x0 };

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    property.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = (u8*)osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value, sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property, &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle add property fail, ret:%x\r\n", SLE_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_USER_DESCRIPTION;
    descriptor.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE;
    descriptor.value = ntf_value;
    descriptor.value_len = sizeof(ntf_value);

    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle add descriptor fail, ret:%x\r\n", SLE_SERVER_LOG, ret);
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    return ERRCODE_SLE_SUCCESS;
}


static errcode_t sle_server_add(void) {
    errcode_t ret;
    sle_uuid_t app_uuid = { 0 };

    osal_printk("%s sle add service in\r\n", SLE_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s sle add service, server_id:%x, service_handle:%x, property_handle:%x\r\n", SLE_SERVER_LOG, g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle add service fail, ret:%x\r\n", SLE_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("%s sle add service out\r\n", SLE_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}


/* device通过uuid向host发送数据：report */
errcode_t sle_server_send_report_by_uuid(const u8* data, u8 len) {
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = { 0 };
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_service_handle;
    param.end_handle = g_property_handle;
    param.value_len = len;
    param.value = (u8*)osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("%s send report new fail\r\n", SLE_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("%s send input report memcpy fail\r\n", SLE_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);
    ret = ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_hdl, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_server_send_report_by_uuid,ssaps_notify_indicate_by_uuid fail :%x\r\n", SLE_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}


/* device通过handle向host发送数据：report */
errcode_t sle_server_send_report_by_handle(const u8* data, u16 len) {
    ssaps_ntf_ind_t param = { 0 };
    u8 receive_buf[UART_BUFF_LENGTH] = { 0 }; /* max receive length. */
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = receive_buf;
    param.value_len = len;
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    return ssaps_notify_indicate(g_server_id, g_sle_conn_hdl, &param);
}


static void sle_connect_state_changed_cbk(u16 conn_id, const sle_addr_t* addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason) {
    u8 sle_connect_state[] = "sle_dis_connect";
    osal_printk("%s connect state changed callback conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n", SLE_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("%s connect state changed callback addr:%02x:**:**:**:%02x:%02x\r\n", SLE_SERVER_LOG, addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_hdl = conn_id;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_sle_conn_hdl = 0;
        g_sle_pair_hdl = 0;
        if (g_sle_server_msg_queue != NULL) {
            g_sle_server_msg_queue(sle_connect_state, sizeof(sle_connect_state));
        }
    }
}


static void sle_pair_complete_cbk(u16 conn_id, const sle_addr_t* addr, errcode_t status) {
    osal_printk("%s pair complete conn_id:%02x, status:%x\r\n", SLE_SERVER_LOG, conn_id, status);
    osal_printk("%s pair complete addr:%02x:**:**:**:%02x:%02x\r\n", SLE_SERVER_LOG, addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    g_sle_pair_hdl = conn_id + 1;
    ssap_exchange_info_t parameter = { 0 };
    parameter.mtu_size = 520;
    parameter.version = 1;
    ssaps_set_info(g_server_id, &parameter);
}


static errcode_t sle_conn_register_cbks(void) {
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = { 0 };
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_conn_register_cbks, sle_connection_register_callbacks fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}


u16 sle_client_is_connected(void) {
    return g_sle_pair_hdl;
}


/* 初始化uuid server */
errcode_t sle_server_init(ssaps_read_request_callback ssaps_read_callback, ssaps_write_request_callback
    ssaps_write_callback) {
    errcode_t ret;

    /* 使能SLE */
    if (enable_sle() != ERRCODE_SUCC) {
        osal_printk("[SLE Server] sle enbale fail !\r\n");
        return -1;
    }

    ret = sle_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_server_init,sle_announce_register_cbks fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_server_init, sle_conn_register_cbks fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_ssaps_register_cbks(ssaps_read_callback, ssaps_write_callback);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_server_init, sle_ssaps_register_cbks fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_server_init,sle_server_add fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_server_init,sle_server_adv_init fail :%x\r\n", SLE_SERVER_LOG, ret);
        return ret;
    }
    osal_printk("%s init ok\r\n", SLE_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}


void sle_server_register_msg(sle_server_msg_queue sle_server_msg) {
    g_sle_server_msg_queue = sle_server_msg;
}

#include "ble_vws.h"
#include "ble_srv_common.h"
#include "sdk_common.h"
#include "app_util_platform.h"

static void on_connect(BleVwsInfo* info, ble_evt_t * p_ble_evt)
{
	info->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void on_disconnect(BleVwsInfo* info, ble_evt_t * p_ble_evt)
{
	(void)p_ble_evt;
	info->conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void on_write(BleVwsInfo* info, ble_evt_t * p_ble_evt)
{
	ble_gatts_evt_write_t* p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ( (p_evt_write->handle == info->word_char_handle.value_handle) && (0 < p_evt_write->len) &&
    		(info->callback != NULL) ) {
    	info->callback(info, p_evt_write->data, p_evt_write->len);
    }
}


void ble_vws_on_ble_evt(BleVwsInfo* info, ble_evt_t * p_ble_evt)
{
	switch (p_ble_evt->header.evt_id) {
	case BLE_GAP_EVT_CONNECTED:
		on_connect(info, p_ble_evt);
		break;

	case BLE_GAP_EVT_DISCONNECTED:
		on_disconnect(info, p_ble_evt);
		break;

	case BLE_GATTS_EVT_WRITE:
		on_write(info, p_ble_evt);
		break;

	default:
		// No implementation needed.
		break;
	}
}

static uint32_t led_char_add(BleVwsInfo* info)
{
	ble_gatts_char_md_t	char_md;
	ble_gatts_attr_t	attr_char_value;
	ble_uuid_t			ble_uuid;
	ble_gatts_attr_md_t	attr_md;

	memset(&char_md, 0, sizeof(char_md));

	char_md.char_props.read   = 1;
	char_md.char_props.write  = 1;
	char_md.p_char_user_desc  = NULL;
	char_md.p_char_pf         = NULL;
	char_md.p_user_desc_md    = NULL;
	char_md.p_cccd_md         = NULL;
	char_md.p_sccd_md         = NULL;

	ble_uuid.type = info->uuid_type;
	ble_uuid.uuid = VWS_UUID_WORD_CHAR;

	memset(&attr_md, 0, sizeof(attr_md));

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.vloc       = BLE_GATTS_VLOC_STACK;
	attr_md.rd_auth    = 0;
	attr_md.wr_auth    = 0;
	attr_md.vlen       = 0;

	memset(&attr_char_value, 0, sizeof(attr_char_value));

	attr_char_value.p_uuid       = &ble_uuid;
	attr_char_value.p_attr_md    = &attr_md;
	attr_char_value.init_len     = 0;
	attr_char_value.init_offs    = 0;
	attr_char_value.max_len      = 8;
	attr_char_value.p_value      = NULL;

	return sd_ble_gatts_characteristic_add(
								info->srv_handle,
								&char_md,
								&attr_char_value,
								&info->word_char_handle);
}

#if 0
static uint32_t button_char_add(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = LBS_UUID_BUTTON_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;

    return sd_ble_gatts_characteristic_add(p_lbs->service_handle,
                                               &char_md,
                                               &attr_char_value,
                                               &p_lbs->button_char_handles);
}
#endif

uint32_t ble_vws_init(BleVwsInfo* info, BleVwsCallback callback)
{
	uint32_t   err_code;
	ble_uuid_t ble_uuid;

	info->conn_handle = BLE_CONN_HANDLE_INVALID;
	info->callback = callback;

	ble_uuid128_t base_uuid = {VWS_UUID_BASE};
	err_code = sd_ble_uuid_vs_add(&base_uuid, &info->uuid_type);
	APP_ERROR_CHECK(err_code);

	ble_uuid.type = info->uuid_type;
	ble_uuid.uuid = VWS_UUID_SERVICE;

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &info->srv_handle);
	APP_ERROR_CHECK(err_code);

#if 0
	err_code = button_char_add(p_lbs, p_lbs_init);
	APP_ERROR_CHECK(err_code);
#endif

	err_code = led_char_add(info);
	APP_ERROR_CHECK(err_code);

	return NRF_SUCCESS;
}

#if 0
uint32_t ble_lbs_on_button_change(ble_lbs_t * p_lbs, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(button_state);

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_lbs->button_char_handles.value_handle;
    params.p_data = &button_state;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_lbs->conn_handle, &params);
}
#endif

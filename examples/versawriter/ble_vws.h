#ifndef _BLE_VWS_H_
#define _BLE_VWS_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/*
#define VWS_UUID_BASE			{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
											0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
*/
#define VWS_UUID_BASE			{ \
									0xB9,0x8F,0xAA,0xC5,0x5E,0x1F,0x0A,0x8A, \
									0x24,0x49,0x01,0xB2,0x00,0x00,0x23,0x07, \
								}\

#define VWS_UUID_SERVICE		0x1001
#define VWS_UUID_WORD_CHAR		0x1002
#define VWS_UUID_BUTTON_CHAR	0x1003


typedef	struct BleVwsInfo_s	BleVwsInfo;
typedef	void (*BleVwsCallback)(const BleVwsInfo* info, const uint8_t* data, uint32_t length);
struct BleVwsInfo_s {
	uint16_t					srv_handle;
	ble_gatts_char_handles_t	word_char_handle;
	ble_gatts_char_handles_t	button_char_handle;
	uint8_t						uuid_type;
	uint16_t					conn_handle;
	BleVwsCallback				callback;
};

extern uint32_t ble_vws_init(BleVwsInfo* info, BleVwsCallback callback);
extern void ble_vws_on_ble_evt(BleVwsInfo* info, ble_evt_t * p_ble_evt);
#if 0
extern uint32_t ble_vws_on_button_change(BleVwsInfo* info, uint8_t button_state);
#endif

#endif // _BLE_VWS_H_

/*
 * application.c
 *
 *  Created on: 2015/07/18
 *      Author: biwa
 */

#include "bwos.h"
#include <stdint.h>
#include <string.h>
#if 0
#include "nordic_common.h"
#include "nrf.h"
#include "nrf51_bitfields.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_gpiote.h"
#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#endif

#include "nrf_gpio.h"

#define	MAX_CHAR_LENGTH			(8)

static void set_gpio(void)
{
    //HIGHで消灯。LOWで点灯。
    nrf_gpio_cfg_output(13);
    nrf_gpio_cfg_output(15);
    nrf_gpio_cfg_output(16);
    nrf_gpio_cfg_output(19);
    nrf_gpio_cfg_output(21);

    nrf_gpio_pin_write(13, 1);
    nrf_gpio_pin_write(15, 1);
    nrf_gpio_pin_write(16, 1);
    nrf_gpio_pin_write(19, 1);
    nrf_gpio_pin_write(21, 1);
    //P0.08 : /RFDET
    //nrf_gpio_cfg_input(8, NRF_GPIO_PIN_PULLUP);
}


/* configuration task */
static int		task_struct[4];

typedef	struct {
	uint8_t		length;
	uint8_t		buff[MAX_CHAR_LENGTH];
} Message;

extern void		bl_main();

static int mutex_request = -1;
static int msgq_id = -1;
static const uint8_t pattern[8][8] = {
#if 0
		{0x01, 0x02, 0x0c, 0x12, 0x12, 0x0c, 0x02, 0x01},
		{0x1f, 0x15, 0x15, 0x15, 0x15, 0x1d, 0x07, 0x03},
		{0x06, 0x09, 0x11, 0x11, 0x11, 0x11, 0x11, 0x03},
		{0x1f, 0x11, 0x11, 0x11, 0x11, 0x09, 0x06, 0x00},
#else
		{0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x11, 0x17, 0x15, 0x1d, 0x00, 0x00},
		{0x00, 0x00, 0x11, 0x15, 0x15, 0x0e, 0x00, 0x00},
		{0x00, 0x00, 0x1c, 0x04, 0x04, 0x1f, 0x04, 0x00},
#endif
};

static Message request_msg = {
	.length = 4,
	.buff[0] = 0,
	.buff[1] = 1,
	.buff[2] = 2,
	.buff[3] = 3,
};

void task1(uint32_t arg0, uint32_t arg1)
{
	bl_main();
}

void task2(uint32_t arg0, uint32_t arg1)
{
    set_gpio();
    for (;;) {
    	Message msg;
    	if ( msgq_recv(msgq_id, &msg, sizeof(msg)) == RT_OK ) {
    		mutex_lock(mutex_request);
    		request_msg = msg;
    		mutex_unlock(mutex_request);
    	}
	}
}

void task3(uint32_t arg0, uint32_t arg1)
{
	for (;;) {
		Message msg;
		mutex_lock(mutex_request);
		msg = request_msg;
		mutex_unlock(mutex_request);
#if 1
		if ( 0 < msg.length ) {
			for ( int msg_ix = 0; msg_ix < msg.length; msg_ix++ ) {
				uint8_t code = msg.buff[msg_ix] & 0x07;
				for ( int ptn_ix = 0; ptn_ix < 8; ptn_ix++ ) {
					uint8_t value = pattern[code][ptn_ix];
					nrf_gpio_pin_write(13, (value & (0x01<<1))?1:0);
					nrf_gpio_pin_write(15, (value & (0x01<<3))?1:0);
					nrf_gpio_pin_write(16, (value & (0x01<<2))?1:0);
					nrf_gpio_pin_write(19, (value & (0x01<<0))?1:0);
					nrf_gpio_pin_write(21, (value & (0x01<<4))?1:0);
					task_tsleep(MSEC(2));
				}
			}
			task_tsleep(MSEC(170-msg.length*8*2));
		}
		else {
			task_tsleep(MSEC(10));
		}
#elif 0
		if ( 0 < msg.length ) {
		for (;;) {
			for ( int ptn_no = 0; ptn_no < 4; ptn_no++ ) {
				for ( int ptn_repeat = 5; 0 < ptn_repeat; ptn_repeat-- ) {
					for ( int ix = 0; ix < 8; ix++ ) {
						uint8_t value = pattern[ptn_no][ix];
						nrf_gpio_pin_write(13, (value & (0x01<<1))?1:0);
						nrf_gpio_pin_write(15, (value & (0x01<<3))?1:0);
						nrf_gpio_pin_write(16, (value & (0x01<<2))?1:0);
						nrf_gpio_pin_write(19, (value & (0x01<<0))?1:0);
						nrf_gpio_pin_write(21, (value & (0x01<<4))?1:0);
						task_tsleep(MSEC(5));
					}
					task_tsleep(MSEC(10));
				}
			}
		}
#elif 0
		for (;;) {
			for ( int ix = 0; ix < 5; ix++ ) {
				uint8_t value = 0x01<<ix;
				nrf_gpio_pin_write(13, (value & (0x01<<1))?1:0);
				nrf_gpio_pin_write(15, (value & (0x01<<3))?1:0);
				nrf_gpio_pin_write(16, (value & (0x01<<2))?1:0);
				nrf_gpio_pin_write(19, (value & (0x01<<0))?1:0);
				nrf_gpio_pin_write(21, (value & (0x01<<4))?1:0);
				task_tsleep(MSEC(10));
			}
		}
#else
		for (;;) {
			nrf_gpio_pin_write(13, 1);
			nrf_gpio_pin_write(15, 1);
			nrf_gpio_pin_write(16, 1);
			nrf_gpio_pin_write(19, 1);
			nrf_gpio_pin_write(21, 1);
			task_tsleep(MSEC(1));
			nrf_gpio_pin_write(13, 0);
			nrf_gpio_pin_write(15, 0);
			nrf_gpio_pin_write(16, 0);
			nrf_gpio_pin_write(19, 0);
			nrf_gpio_pin_write(21, 0);
			task_tsleep(MSEC(9));
		}
#endif
	}
}

void notify_uart(const uint8_t* buff, uint32_t length)
{
	if ( 0 <= msgq_id ) {
		Message msg;
		if ( MAX_CHAR_LENGTH < length ) {
			length = MAX_CHAR_LENGTH;
		}
		memcpy(msg.buff, buff, length);
		msg.length = length;
    	msgq_send(msgq_id, &msg, sizeof(msg));
	}
}

TaskCreateInfo	task_info[] = {
		{"TASK1", TASK_ACT, task1, 0, 512, 0, 5, (void*)128},
		{"TASK2", TASK_ACT, task2, 0, 512, 0, 6, (void*)128},
		{"TASK3", TASK_ACT, task3, 0, 512, 0, 6, (void*)128},
};

void main_task(void)
{
	int ix;
	for ( ix=0; ix<arrayof(task_info); ix++ ) {
		task_struct[ix] = task_create(&task_info[ix]);
	}
	mutex_request = mutex_create();
    msgq_id = msgq_create(64);
	task_sleep();
}

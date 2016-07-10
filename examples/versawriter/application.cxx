/*
 * application.c
 *
 *  Created on: 2015/07/18
 *      Author: biwa
 */

#include "bwos.h"
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "fonts.h"

#define	MAX_CHAR_LENGTH			(10)

typedef	struct {
	uint8_t		length;
	uint8_t		buff[MAX_CHAR_LENGTH];
} Message;

extern "C" void		bl_main();

/* LED点灯位置とGPIO番号対応表 */
static const uint8_t led_reg[] = { 9, 11, 12, 13, 15, 16, 19 };


/* configuration task */
static int task_struct[4];

static int mutex_request = -1;
static int msgq_id = -1;
static int sync_flag = -1;

/* 表示する文字列格納用 */
static Message request_msg;		/* 文字列更新要求データ */
static Message target_msg = {	/* 表示中のデータ */
		4, "ABCD"
};

/* GPIO初期化 */
static void gpio_init(void)
{
	/* LED用GPIOを 出力設定/初期値消灯　に設定 */
	for ( int ix = 0; ix < arrayof(led_reg); ix++ ) {
		nrf_gpio_cfg_output(led_reg[ix]);
	    nrf_gpio_pin_write(led_reg[ix], 0);
	}

	/* 回転原点検出用 GPIOE(GPIO割り込み)設定 */
	irq_set_enable(GPIOTE_IRQn, IRQ_DISABLE, 0x80);
	NRF_GPIOTE->CONFIG[0] = (0x1u << 16) | (21u << 8) | (1u << 0);
	NRF_GPIOTE->EVENTS_IN[0] = 0;
	NRF_GPIOTE->INTENSET = 0x01;
	irq_set_enable(GPIOTE_IRQn, IRQ_ENABLE, 0x80);
}

/* LED表示設定 */
static void led_output(uint32_t pattern)
{
	for ( int ix = 0; ix < arrayof(led_reg); ix++ ) {
		nrf_gpio_pin_write(led_reg[ix], (pattern & (1u << ix)) ? 1 : 0);
	}
}


/* BLE実行用タスク */
static void task1(void* arg0, void* arg1)
{
	bl_main();
}

/* BLEモジュールからの通知データを処理するタスク */
static void task2(void* arg0, void* arg1)
{
	uint32_t pattern = 0;
    for (;;) {
    	Message msg;
    	if ( msgq_recv(msgq_id, &msg, sizeof(msg)) == RT_OK ) {
    		led_output(pattern);
    		pattern++;
    		mutex_lock(mutex_request);
    		request_msg = msg;
    		mutex_unlock(mutex_request);
    	}
	}
}

/* LED表示用タスク */
static void main_apps()
{
	gpio_init();
	for (;;) {
		Message msg;
		mutex_lock(mutex_request);
		msg = request_msg;
		mutex_unlock(mutex_request);
		/* 原点検知待ち */
    	uint32_t ret_pattern;
    	flag_wait(sync_flag, 0x0001, FLAG_OR|FLAG_CLR, &ret_pattern);
		if ( 0 < msg.length ) {
			for ( int msg_ix = 0; msg_ix < msg.length; msg_ix++ ) {
				uint8_t code = msg.buff[msg_ix] - 0x20;
				for ( int ptn_ix = 0; ptn_ix < arrayof(Font[0]); ptn_ix++ ) {
					led_output(Font[code][ptn_ix]);
					task_tsleep(USEC(2000));
				}
				/* 文字間ディレイ */
				led_output(0);
				task_tsleep(USEC(3000));
			}
		}
	}
}

extern "C" void notify_uart(const uint8_t* buff, uint16_t length)
{
		Message msg;
		if ( MAX_CHAR_LENGTH < length ) {
			length = MAX_CHAR_LENGTH;
		}
		memcpy(msg.buff, buff, length);
		msg.length = length;
    	msgq_send(msgq_id, &msg, sizeof(msg));
}

TaskCreateInfo	task_info[] = {
		{"TASK1", TASK_ACT, task1, 512, 0, 5, (void*)128},
		{"TASK2", TASK_ACT, task2, 512, 0, 6, (void*)128},
};

void main_task(void)
{
	int ix;
	for ( ix=0; ix<arrayof(task_info); ix++ ) {
		task_struct[ix] = task_create(&task_info[ix]);
	}
	mutex_request = mutex_create();
	msgq_id = msgq_create(64);
	sync_flag = flag_create();
	main_apps();
	task_sleep();
}

/* GPIOE割り込みハンドラ */
extern "C" void GPIOTE_IRQHandler(void)
{
	NRF_GPIOTE->EVENTS_IN[0] = 0;
    flag_set(sync_flag, 0x0001);
}

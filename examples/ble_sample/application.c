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

#define	MAX_CHAR_LENGTH			(8)

/* configuration task */
static int		task_struct[4];

typedef	struct {
	uint8_t		length;
	uint8_t		buff[MAX_CHAR_LENGTH];
} Message;

extern void		bl_main();

static int mutex_request = -1;
static int msgq_id = -1;
static int sync_flag = -1;

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

//static const uint8_t led_reg[] = { 8, 9, 11, 12, 13, 15, 16, 19};
static const uint8_t led_reg[] = { 19, 16, 15, 13, 12, 11, 9, 8 };

static void set_gpio(void)
{
    //HIGHで消灯。LOWで点灯。
	for ( int ix = 0; ix < arrayof(led_reg); ix++ ) {
		nrf_gpio_cfg_output(led_reg[ix]);
	    nrf_gpio_pin_write(led_reg[ix], 0);
	}

	//nrf_gpio_cfg_output(19);
    //nrf_gpio_pin_write(19, 0);
    //P0.08 : /RFDET
    //nrf_gpio_cfg_input(8, NRF_GPIO_PIN_PULLUP);

	__irq_set_enable(GPIOTE_IRQn, IRQ_DISABLE, 0x80);
	NRF_GPIOTE->CONFIG[0] = (0x1u << 16) | (21u << 8) | (1u << 0);
	NRF_GPIOTE->EVENTS_IN[0] = 0;
	NRF_GPIOTE->INTENSET = 0x01;
	__irq_set_enable(GPIOTE_IRQn, IRQ_ENABLE, 0x80);
}

static void led_output_line(uint32_t pattern)
{
	for ( int ix = 0; ix < arrayof(led_reg); ix++ ) {
		nrf_gpio_pin_write(led_reg[ix], (pattern & (1u << ix)) ? 1 : 0);
	}
}

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
    set_gpio();
	for (;;) {
		Message msg;
		mutex_lock(mutex_request);
		msg = request_msg;
		mutex_unlock(mutex_request);
		/* 原点検知待ち */
    	uint32_t ret_pattern;
    	flag_wait(sync_flag, 0x0001, FLAG_OR|FLAG_CLR, &ret_pattern);
#if 1
		if ( 0 < msg.length ) {
			for ( int msg_ix = 0; msg_ix < msg.length; msg_ix++ ) {
				uint8_t code = msg.buff[msg_ix] & 0x07;
				for ( int ptn_ix = 0; ptn_ix < 8; ptn_ix++ ) {
					led_output_line(pattern[code][ptn_ix]);
					task_tsleep(USEC(1500));
				}
			}
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
#elif 0
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

void GPIOTE_IRQHandler(void)
{
static int flag = 0;
	NRF_GPIOTE->EVENTS_IN[0] = 0;
    //nrf_gpio_pin_write(19, (flag & 0x01) ? 1 : 0);
    flag++;
    flag_set(sync_flag, 0x0001);
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
		{"TASK3", TASK_ACT, task3, 0, 512, 0, 4, (void*)128},
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
	task_sleep();
}

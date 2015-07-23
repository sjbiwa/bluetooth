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

extern void		bl_main();

void task1(uint32_t arg0, uint32_t arg1)
{
	bl_main();
	task_sleep();
}

static int sem_uart_event = -1;
static int msgq_id = -1;

void task2(uint32_t arg0, uint32_t arg1)
{
    set_gpio();
    msgq_id = msgq_create(64);
    for (;;) {
    	uint8_t value;
    	if ( msgq_recv(msgq_id, &value, 1) == RT_OK ) {
            nrf_gpio_pin_write(13, (value & (0x01<<4))?1:0);
            nrf_gpio_pin_write(15, (value & (0x01<<3))?1:0);
            nrf_gpio_pin_write(16, (value & (0x01<<2))?1:0);
            nrf_gpio_pin_write(19, (value & (0x01<<1))?1:0);
            nrf_gpio_pin_write(21, (value & (0x01<<0))?1:0);
            task_tsleep(MSEC(50));
    	}
	}
}

void notify_uart(const uint8_t* buff, uint32_t length)
{
	if ( 0 <= msgq_id ) {
		for ( ; 0 < length; length--, buff++ ) {
	    	msgq_send(msgq_id, buff, 1);
		}
	}
}

TaskCreateInfo	task_info[] = {
		{"TASK1", TASK_ACT, task1, 0, 512, 0, 5, (void*)128},
		{"TASK2", TASK_ACT, task2, 0, 512, 0, 6, (void*)128},
};

void main_task(void)
{
	int ix;
	for ( ix=0; ix<arrayof(task_info); ix++ ) {
		task_struct[ix] = task_create(&task_info[ix]);
	}

	task_sleep();
}

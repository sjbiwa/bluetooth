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
    //P0.21にLED接続。
    //HIGHで消灯。LOWで点灯。
    nrf_gpio_cfg_output(21);
    nrf_gpio_pin_write(21, 1);
    //P0.08 : /RFDET
    nrf_gpio_cfg_input(8, NRF_GPIO_PIN_PULLUP);
}


/* configuration task */
static int		task_struct[4];

extern void		bl_main();

void task1(uint32_t arg0, uint32_t arg1)
{
	bl_main();
	task_sleep();
}

void task2(uint32_t arg0, uint32_t arg1)
{
    set_gpio();
    for (;;) {
        nrf_gpio_pin_write(21, 1);
		task_tsleep(MSEC(200));
        nrf_gpio_pin_write(21, 0);
		task_tsleep(MSEC(200));
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

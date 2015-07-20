/*
 * application.c
 *
 *  Created on: 2015/07/18
 *      Author: biwa
 */

#include "bwos.h"
#include <stdint.h>
#include <string.h>
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

#if 0
#define SPI_MASTER_0_ENABLE /**< Enable SPI_MASTER_0 */

void spi_master_event_handler(spi_master_evt_t spi_master_evt)
{
    switch (spi_master_evt.evt_type)
    {
        case SPI_MASTER_EVT_TRANSFER_COMPLETED:
            //Data transmission is ended successful. 'rx_buffer' has data received from SPI slave.

            transmission_completed = true;
            break;

        default:
            //No implementation needed.
            break;
    }
}
void spi_master_init(void)
{
    //Structure for SPI master configuration, initialized by default values.
    spi_master_config_t spi_config = SPI_MASTER_INIT_DEFAULT;

    //Configure SPI master.
    spi_config.SPI_Pin_SCK  = SPIM0_SCK_PIN;
    spi_config.SPI_Pin_MISO = SPIM0_MISO_PIN;
    spi_config.SPI_Pin_MOSI = SPIM0_MOSI_PIN;
    spi_config.SPI_Pin_SS   = SPIM0_SS_PIN;

    //Initialize SPI master.
    uint32_t err_code = spi_master_open(SPI_MASTER_0, &spi_config);
    if (err_code != NRF_SUCCESS)
    {
        //Module initialization failed. Take recovery action.
    }

    //Register SPI master event handler.
    spi_master_evt_handler_reg(SPI_MASTER_0, spi_master_event_handler);
}
#endif

/* configuration task */
static int		task_struct[4];

extern void		bl_main();

void task1(uint32_t arg0, uint32_t arg1)
{
	bl_main();
}

uint32_t idle_counter = 0;
void task2(uint32_t arg0, uint32_t arg1)
{
	for (;;) {
		task_tsleep(SEC(5));
		task_tsleep(SEC(5));
		idle_counter++;
	}
}

TaskCreateInfo	task_info[] = {
		{"TASK1", TASK_ACT, task1, 0, 512, 0, 31, (void*)128},
		{"TASK2", TASK_ACT, task2, 0, 512, 0, 10, (void*)128},
};

void main_task(void)
{
	int ix;
	for ( ix=0; ix<arrayof(task_info); ix++ ) {
		task_struct[ix] = task_create(&task_info[ix]);
	}

	task_sleep();
}

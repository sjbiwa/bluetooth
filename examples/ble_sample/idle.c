/*
 * idle.c
 *
 *  Created on: 2015/07/01
 *      Author: biwa
 */
#include <stdint.h>
#include <string.h>
#include "app_util_platform.h"

#include "kernel.h"
#include "arm.h"

/*
 * 	idle_task
 * 	  call in DISABLE_IRQ
 */
void
idle_task(void)
{
	for (;;) {
		__wfe();
	}
}

void fault_handler(uint32_t id, uint32_t* sp)
{
	for (;;);
}

void _nmi_entry(void)
{
	fault_handler(0, 0);
}
void _hard_fault_entry(void)
{
	fault_handler(0, 0);
}
void _mem_manage_entry(void)
{
	fault_handler(0, 0);
}
void _bus_fault_entry(void)
{
	fault_handler(0, 0);
}
void _usage_fault_entry(void)
{
	fault_handler(0, 0);
}

void _debug_entry(void)
{
	fault_handler(0, 0);
}

void _systick_entry(void)
{
	timer_handler(0,0);
}

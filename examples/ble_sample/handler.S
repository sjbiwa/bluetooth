	.syntax unified
	.thumb
	.thumb_func

/*
** bwos boot.S
*/
#include "arm.h"

/* Vector */
	.text
	.balign	4

	.globl	main
	.type   main, %function
main:
	ldr		r0, =stack_idle
	msr		PSP, r0
	movs	r0, #0x02
	msr		CONTROL, r0
	cpsid	i
	bl		system_init
	bl		startup

	.globl	PendSV_Handler
	.type   PendSV_Handler, %function
PendSV_Handler:
	b		_pendsvc_entry

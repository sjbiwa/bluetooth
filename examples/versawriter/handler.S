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

	.globl	_start
	.type   _start, %function
_start:
	/* bss clear */
	ldr		r0, =__bss_start__
	ldr		r1, =__bss_end__
	movs	r2, #0
_bss_loop:
	cmp		r0, r1
	bhs		_bss_done
#if __ARM_ARCH == 7
	str		r2, [r0], #4
#else
	str		r2, [r0]
	adds	r0, #4
#endif
	b		_bss_loop
_bss_done:

	bl		main

_loop_:
	b		_loop_


	.globl	main
	.type   main, %function
main:
	ldr		r0, =stack_startup
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

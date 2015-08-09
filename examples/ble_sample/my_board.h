/*
 * my_board.h
 *
 *  Created on: 2015/07/01
 *      Author: biwa
 */

#ifndef MY_BOARD_H_
#define MY_BOARD_H_

#include "stdint.h"
#include "cmsis.h"

extern char					__heap_end;
#define	END_MEM_ADDR		(&__heap_end-1)

#define	IRQ_NUM				(32+16)

#define	CACHE_LINE_SIZE		(64)

#define	CPU_CLOCK			(MAX_CLOCK_FREQ)/* CPUクロック */

#define	TICK_TIME			(1)		/* tick周期 10ms */

/*** Tick管理パラメータ ***/
typedef	uint64_t			TimeSpec;	/* OS内部での時間単位 */
typedef	uint32_t			TimeOut;	/* APIで指定するタイムアウト単位 */

/* TIME_UNIT値に追従すること (30.517us) */
#if 1
#define	SEC(n)		((n)*32768)
#define	MSEC(n)		(((n)*1000)/30)
#define	USEC(n)		(((n)*1000)/30517)
#define	NSEC(n)		(1)
#else
#define	SEC(n)		((n)*1000000)
#define	MSEC(n)		((n)*1000))
#define	USEC(n)		(n)
#define	NSEC(n)		(1)
#endif

#endif /* MY_BOARD_H_ */

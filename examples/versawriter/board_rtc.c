/*
 * timer.c
 *
 *  Created on: 2015/07/01
 *      Author: biwa
 */
#include "kernel.h"
#include "common.h"
#include "cmsis.h"

#define	NO_TMOUT_COUNT		(0xFFFFFFFFFFFFFFFFLL)
#define	NORMAL_TMOUT_COUNT	(327680)

/* Tick処理 */
static TimeSpec		tick_count;		/* Tick Counter */
static TimeSpec		tmout_count;	/* Timeout Counter value */
/* DUMMY */
void timer_handler(void)
{
}

/* tick_countを最新に更新 */
static inline void update_tick_count(void)
{
	tick_count += NRF_RTC1->COUNTER;
}

static void update_timer(TimeSpec tm_diff)
{
	if ( NORMAL_TMOUT_COUNT < tm_diff ) {
		tm_diff = NORMAL_TMOUT_COUNT;
	}
	NRF_RTC1->CC[0] = tm_diff;
	NRF_RTC1->TASKS_CLEAR = 1;


}

void RTC1_IRQHandler(void)
{
	uint32_t irq_state = irq_save();
	NRF_RTC1->EVENTS_COMPARE[0] = 0;  /* 割り込み要求ビットクリア */
	/* TICK値更新 */
	update_tick_count();
	if ( tmout_count <= tick_count ) {
		TimeSpec tm_diff = tick_count - tmout_count;
		update_timer(tm_diff);

		tmout_count = NO_TMOUT_COUNT;
		irq_restore(irq_state);
		/* OSのタイムアウト処理呼び出し */
		task_tick();
	}
	else {
		TimeSpec tm_diff = tmout_count - tick_count;
		update_timer(tm_diff);

		irq_restore(irq_state);
	}

}

/* TICKカウンタの取得 */
TimeSpec get_tick_count(void)
{
	TimeSpec ret;
	uint32_t irq_state = irq_save();
	ret = tick_count + NRF_RTC1->COUNTER;
	irq_restore(irq_state);
	return ret;
}

/* 最短のタイムアウト時間通知 (tickless対応用) */
void update_first_timeout(TimeSpec tmout)
{
	TimeSpec ret;
	uint32_t irq_state = irq_save();
	tmout_count = tmout;
	/* TICK値更新 */
	update_tick_count();
	if ( tmout_count <= tick_count ) {
		/* すでに過ぎているのでTIMER割り込みONする */
		NVIC->ISPR[RTC1_IRQn/32] = 0x1u << (RTC1_IRQn % 32);
	}
	else {
		TimeSpec tm_diff = tmout_count - tick_count;
		update_timer(tm_diff);
	}
	irq_restore(irq_state);
}

void
arch_timer_init(uint32_t cpuid)
{
	__irq_set_enable(RTC1_IRQn, IRQ_DISABLE, 0);

	/* Enable LFCLK */
	NRF_CLOCK->TASKS_LFCLKSTART = 1;

	/* PRESCALE設定 (約1ms毎にカウントアップ) */
	NRF_RTC1->PRESCALER = 0;

	NRF_RTC1->INTENSET = 0x1u<<16;
	NRF_RTC1->TASKS_START = 1;

	/* 標準タイムアウト値をMATCHレジスタに設定 */
	update_timer(NO_TMOUT_COUNT);

	tmout_count = NO_TMOUT_COUNT;

	__irq_set_enable(RTC1_IRQn, IRQ_ENABLE, 0);

}

/// \file clock.c
/*
  -----------------------------------
  Create date: 2013-11-26 21:06
  Modified date: 2013-12-04 13:37
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC void clock_handler(u32 irq)
{
	++ticks;
	--p_proc_ready->ticks;
	if(k_reenter != 0)
		return;
	if(p_proc_ready->ticks > 0)
		return;
	schedule();
}

PUBLIC void init_clock()
{
	out_byte(TIMER_MODE,RATE_GENERATOR);
	out_byte(TIMER0,(u8)(TIMER_FREQ/HZ));
	out_byte(TIMER0,(u8)((TIMER_FREQ/HZ) >> 8));

	put_irq_handler(CLOCK_IRQ,clock_handler);
	enable_irq(CLOCK_IRQ);
}

PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();

        while(((get_ticks() - t) * 1000 / HZ) < milli_sec);
}

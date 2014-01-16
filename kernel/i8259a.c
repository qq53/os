/// \file i8259a.c
/*
  -----------------------------------
  Create date: 2013-11-24 00:12
  Modified date: 2013-12-01 19:54
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC void init_8259A()
{
	///ICW1
	out_byte(INT_M_CTL,0x11);
	out_byte(INT_S_CTL,0x11);

	///set INT entrypoint ICW2
	out_byte(INT_M_CTLMASK,INT_VECTOR_IRQ0);
	out_byte(INT_S_CTLMASK,INT_VECTOR_IRQ8);

	///ICW3
	out_byte(INT_M_CTLMASK,0x4);
	out_byte(INT_S_CTLMASK,0x2);

	///ICW4
	out_byte(INT_M_CTLMASK,0x1);
	out_byte(INT_S_CTLMASK,0x1);

	///OCW1
	out_byte(INT_M_CTLMASK,0xFF);
	out_byte(INT_S_CTLMASK,0xFF);

	int i;
	for(i=0;i<NR_IRQ;++i)
		irq_table[i] = (irq_handler)spurious_irq;
}


PUBLIC void spurious_irq(u16 irq)
{
	disp_str("spurious_irq :");
	disp_int(irq);
	disp_str("\n");
}

PUBLIC void put_irq_handler(u16 irq,irq_handler handler)
{
	disable_irq(irq);
	irq_table[irq] = handler;
}

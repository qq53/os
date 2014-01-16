/// \file main.c
/*
  -----------------------------------
  Create date: 2013-11-26 01:05
  Modified date: 2014-01-16 16:17
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

PUBLIC	void init_proc(u16 index,char *name,task_call pfun,u32 pid,u32 flags,u32 priority, \
		u8 rpl)
{
	///前面数字代表选择子,LDT只有两个描述符
	proc_table[index].regs.cs		=	0 | SA_TIL | rpl;
	proc_table[index].regs.ds		=	8 | SA_TIL | rpl;
	proc_table[index].regs.es		=	8 | SA_TIL | rpl;
	proc_table[index].regs.fs		=	8 | SA_TIL | rpl;
	proc_table[index].regs.ss		=	8 | SA_TIL | rpl;
	proc_table[index].regs.gs		=	SELECTOR_KERNEL_GS | rpl;
	proc_table[index].regs.eip		=	(u32)pfun;
	proc_table[index].regs.esp		=	(u32)task_stack + STACK_SIZE_TOTAL * (index+1);
	proc_table[index].regs.eflags	=	flags;
	proc_table[index].pid			=	pid;
	memcpy(proc_table[index].p_name,name,PROC_NAME_SIZE);
	proc_table[index].ticks = proc_table[index].priority = priority;

	proc_table[index].p_flags		=	0;
	proc_table[index].p_msg			=	NULL;
	proc_table[index].p_recvfrom	=	NO_TASK;
	proc_table[index].p_sendto		=	NO_TASK;
	proc_table[index].has_int_msg	=	FALSE;
	proc_table[index].q_sending		=	NULL;
	proc_table[index].next_sending	=	NULL;
}

void TestA(u32 lParam)
{
	while(1)
	{
		//printl("<Ticks:%x>",get_ticks());
		milli_delay(2);
	}
}

PUBLIC void kernel_main()
{
	disp_str("kernel_main begin\n");

	memcpy(&ldts_task[0],&gdt[SELECTOR_KERNEL_CS>>3],sizeof(DESCRIPTOR));
	ldts_task[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
	memcpy(&ldts_task[1],&gdt[SELECTOR_KERNEL_DS>>3],sizeof(DESCRIPTOR));
	ldts_task[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;

	memcpy(&ldts_user[0],&gdt[SELECTOR_KERNEL_CS>>3],sizeof(DESCRIPTOR));
	ldts_user[0].attr1 = DA_C | PRIVILEGE_USER << 5;
	memcpy(&ldts_user[1],&gdt[SELECTOR_KERNEL_DS>>3],sizeof(DESCRIPTOR));
	ldts_user[1].attr1 = DA_DRW | PRIVILEGE_USER << 5;
	
	proc_index	= -1;
	proc_pid	= 100; 

	init_proc(++proc_index,"TTY",task_tty,++proc_pid,0x1202,15,RPL_TASK);
	init_proc(++proc_index,"SYS",task_sys,++proc_pid,0x1202,15,RPL_TASK);//eflags = 0x1202 IF=1 IOPL=1
	init_proc(++proc_index,"A",TestA,++proc_pid,0x202,5,RPL_USER);

	ldt_sel = SELECTOR_LDT_TASK;
	p_proc_ready = &proc_table[0];
	k_reenter = 0;
	ticks = 0;

	init_clock();

	restart();

	while(1){}
}

PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	va_list arg = (va_list)((char *)&fmt + 4);/*add 4 is fmt size,to variate list */

	i = vsprintf(buf, fmt, arg);

	printl("%c panic %s",MAG_CH_PANIC, buf);

	__asm__ __volatile__("ud2");
}

PUBLIC int get_ticks()
{
	MESSAGE msg;

	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.u.mess1;
}

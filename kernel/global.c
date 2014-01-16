/// \file global.c
/*
  -----------------------------------
  Create date: 2013-11-23 22:27
  Modified date: 2014-01-15 21:19
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#define _GLOBAL_VARIABLES_

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC  u16				ldt_sel;
PUBLIC	DESCRIPTOR		ldts_task[LDT_SIZE];
PUBLIC	DESCRIPTOR		ldts_user[LDT_SIZE];

PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	char			task_stack[STACK_SIZE_TOTAL*NR_TASKS];

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {sys_printx,sys_sendrec};

PUBLIC	int				proc_index;
PUBLIC	int				proc_pid;

PUBLIC	TTY				tty_table[NR_CONSOLES];
PUBLIC  CONSOLE			console_table[NR_CONSOLES];

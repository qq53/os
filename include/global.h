/// \file global.h
/*
  -----------------------------------
  Create date: 2013-11-23 22:25
  Modified date: 2014-01-15 21:19
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#ifdef _GLOBAL_VARIABLES_
#undef EXTERN
#define EXTERN
#endif

EXTERN	int				disp_pos;
EXTERN	u8				gdt_ptr[6];
EXTERN	DESCRIPTOR		gdt[GDT_SIZE];
EXTERN	u8				idt_ptr[6];
EXTERN	GATE			idt[IDT_SIZE];

EXTERN	TSS				tss;
EXTERN	PROCESS*		p_proc_ready;

EXTERN	u32				k_reenter;

EXTERN	DESCRIPTOR		ldts_task[];	
EXTERN	DESCRIPTOR		ldts_user[];	
EXTERN	u16				ldt_sel;
EXTERN	u8				current_pri;

EXTERN	PROCESS			proc_table[];
EXTERN	char			task_stack[];
EXTERN	irq_handler		irq_table[];

EXTERN  int				proc_index;
EXTERN  int				proc_pid;

EXTERN	system_call		sys_call_table[];	

EXTERN	int				ticks;

EXTERN	TTY				tty_table[];
EXTERN  CONSOLE			console_table[];

EXTERN	u32				nr_current_console;

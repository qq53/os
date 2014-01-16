/// \file proc.h
/*
  -----------------------------------
  Create date: 2013-11-23 22:28
  Modified date: 2014-01-15 21:25
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
typedef struct s_stackframe
{
	u32 gs;
	u32 fs;
	u32 es;
	u32 ds;
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 kernel_esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 retaddr;
	u32 eip;
	u32 cs;
	u32 eflags;
	u32	esp;
	u32 ss;
}STACK_FRAME;

#define PROC_NAME_SIZE		32

typedef struct s_proc
{
	STACK_FRAME regs;
	
	u32 pid;
	char p_name[PROC_NAME_SIZE];
	int ticks;
	int priority;
	int p_flags;
	MESSAGE *p_msg;
	int p_sendto;
	int p_recvfrom;
	int has_int_msg;
	struct s_proc *q_sending;
	struct s_proc *next_sending;
}PROCESS;

#define NR_TASKS	3

#define STACK_SIZE_TOTAL	0x8000
#define STACK_PROC_SIZE		STACK_SIZE_TOTAL

#define procpri(x) (x->regs.cs & 0x3)

#define proc2pid(x) (x - proc_table)

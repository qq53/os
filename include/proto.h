/// \file proto.h
/*
  -----------------------------------
  Create date: 2013-11-23 23:07
  Modified date: 2014-01-11 00:12
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
///klib.asm
PUBLIC void	out_byte(u16 port,u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_str(char *info);
PUBLIC void disp_color_str(char *info,u32 color);
PUBLIC void enable(u16);
PUBLIC void disable(u16);

///i8259A
PUBLIC void put_irq_handler(u16,irq_handler);
PUBLIC void spurious_irq(u16);

///protect.c
PUBLIC u32 seg2phys(u16 seg);

///klib.c
PUBLIC void disp_int(int);

///kernel.asm
PUBLIC void restart();

///main.c
PUBLIC int	get_ticks();
PUBLIC void panic(const char*, ...);

///clock.c
PUBLIC void milli_delay(int);
PUBLIC void init_clock();

///printf.c
PUBLIC int printf(const char*, ...);
#define printl printf

///proc.c
PUBLIC void schedule();
PUBLIC int sys_sendrec(int, int, MESSAGE*, PROCESS*);
PUBLIC int sys_printx(int, int, char*, PROCESS*);

///syscall.asm
PUBLIC void sys_call();
PUBLIC int sendrec(int, int, MESSAGE*);
PUBLIC int printx(char*);

///keyboard.c
PUBLIC void init_keyboard();

///tty.c
PUBLIC void task_tty();
PUBLIC void in_process(TTY*,u32);

///systask.c
PUBLIC void task_sys();

///misc.c
PUBLIC void spin(char*);

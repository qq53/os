/// \file tty.c
/*
  -----------------------------------
  Create date: 2013-11-30 21:39
  Modified date: 2014-01-14 22:06
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "keyboard.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

#define	TTY_FIRST	(tty_table)
#define	TTY_END		(tty_table + NR_CONSOLES)

PRIVATE void init_tty(TTY*);
PRIVATE void tty_do_read(TTY*);
PRIVATE void tty_do_write(TTY*);

PUBLIC void task_tty()
{
	TTY *p_tty;

	init_keyboard();
	for(p_tty=TTY_FIRST;p_tty<TTY_END;++p_tty)
		init_tty(p_tty);
	select_console(0);

	while(1)
	{
		for(p_tty=TTY_FIRST;p_tty<TTY_END;++p_tty)
		{
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}

PUBLIC void init_tty(TTY* p_tty)
{
	p_tty->count = 0;
	p_tty->head = p_tty->tail = -1;

	init_console(p_tty);
}

void put_key(TTY* p_tty,u32 key)
{
	if(p_tty->count < TTY_IN_BYTES)
	{
		if(p_tty->head >= TTY_IN_BYTES - 1)
			p_tty->head = -1;
		p_tty->in_buf[++p_tty->head] = key;
		++p_tty->count;
	}
}

PUBLIC void in_process(TTY* p_tty,u32 key)
{
	if((key & FLAG_EXT) == 0)
	{
		put_key(p_tty,key);
	}
	else
	{
		int raw_code = key & MASK_RAW;
		switch(raw_code)
		{
			case PAGEUP:
				scroll_screen(p_tty->p_console,SCR_UP);
				break;
			case PAGEDOWN:
				scroll_screen(p_tty->p_console,SCR_DN);
				break;
			case F1:
			case F2:
			case F3:
				if(key & FLAG_ALT_L || key & FLAG_ALT_R)
					select_console(raw_code - F1);
				break;
			case ENTER:
				put_key(p_tty,'\n');
				break;
			case BACKSPACE:
				put_key(p_tty,'\b');
				break;
			default:
				break;
		}
	}
}

PRIVATE void tty_do_read(TTY* p_tty)
{
	if(is_current_console(p_tty->p_console))
		keyboard_read(p_tty);
}

PRIVATE void tty_do_write(TTY* p_tty)
{
	if(p_tty->count)
	{
		if(p_tty->tail >= TTY_IN_BYTES -1)
			p_tty->tail = -1;
		char ch = p_tty->in_buf[++p_tty->tail];
		--p_tty->count;

		out_char(p_tty->p_console,ch);
	}
}

PUBLIC int sys_write(char *buf,int len)
{
	char *p = buf;
	int i = len;

	while(i)
	{
		out_char(&console_table[nr_current_console],*p++);
		--i;
	}
}

PUBLIC int sys_printx(int _unused1, int _unused2, char *s, PROCESS *p_proc)
{
	char *p;
	char ch;

	char reenter_err[] = "? k_reenter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;

	if (k_reenter == 0)
		p = s;
	else if (k_reenter > 0)
		p = s;
	else
		p = reenter_err;
	
	if((*p == MAG_CH_PANIC) ||
		(*p == MAG_CH_ASSERT && procpri(p_proc) == PRIVILEGE_TASK))
	{
		disable_int();
		char *v = (char *)V_MEM_BASE;
		const char *q = p + 1;//skip the magic char

		while(v < (char)(V_MEM_BASE + V_MEM_SIZE))
		{
			*v++ = *q++;
			*v++ = RED_CHAR;
			if(!*q)
			{
				while(((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16))
				{
					v++;
					*v++ = GRAY_CHAR;
				}
				q = p + 1;
			}
		}
		__asm__ __volatile__("hlt");
	}

	while(*p != 0)
	{
		if(*p == MAG_CH_PANIC || *p == MAG_CH_ASSERT)
			continue;
		out_char(&console_table[nr_current_console],*p);
		++p;
	}

	return 0;
}

/// \file console.c
/*
  -----------------------------------
  Create date: 2013-12-01 19:07
  Modified date: 2014-01-05 15:39
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

PRIVATE int color[] = {4,5,5,5,6,7,8,8,7,6,5,5,5,4};
PRIVATE	void set_cursor(u32	position);
PRIVATE void set_video_start_addr(u32);
PUBLIC void scroll_screen(CONSOLE*,int);

PUBLIC int is_current_console(CONSOLE *p_con)
{
	return (p_con == &console_table[nr_current_console]);
}

PRIVATE void flush(CONSOLE* p_con)
{
	set_cursor(p_con->cursor);
    set_video_start_addr(p_con->current_start_addr);
}

PRIVATE u8 cursor_color(CONSOLE *p_con)
{
	int i,j = -1;

	i = (p_con->cursor - p_con->origin_addr) % 80 + 1;
	
	while(i > 0)
	{
		i -= color[++j];
	}
	++j;
	if(j > 7)
		j = 15 - j;
	return (j | 8); ///除去黑色 加亮
}

PUBLIC void out_char(CONSOLE *p_con,char ch)
{
	u8* p_mem = (u8*)(V_MEM_BASE + (p_con->cursor << 1));

	switch(ch)
	{
	case '\n':
		if(p_con->cursor < p_con->origin_addr + p_con->mem_limit - SCREEN_WIDTH)
		{
			p_con->cursor = p_con->origin_addr + SCREEN_WIDTH * 
				((p_con->cursor - p_con->origin_addr) / SCREEN_WIDTH + 1);
		}
		break;
	case '\b':
		if(p_con->cursor > p_con->origin_addr)
		{
			p_mem -= 2;
			*p_mem++ = ' ';
			*p_mem = DEFAULT_CHAR_COLOR;
			--p_con->cursor;
		}
		break;
	default:
		*p_mem++ = ch;
		*p_mem = cursor_color(p_con);
		++p_con->cursor;
		if(p_con->cursor > p_con->current_start_addr + SCREEN_SIZE)
			ch = '\n';
		break;
	}
	if((ch == '\n' || ch == '\b') && p_con->cursor > p_con->origin_addr && 
			p_con->cursor < p_con->origin_addr + p_con->mem_limit)
	{
		if(p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE)
				scroll_screen(p_con,LIN_DN);
		else if(p_con->cursor <= p_con->current_start_addr)
				scroll_screen(p_con,LIN_UP);
	}
	flush(p_con);
}

PRIVATE void set_cursor(u32 position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position>>8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr>>8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}

PUBLIC void init_console(TTY *p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = &console_table[nr_tty];

	int v_mem_size = V_MEM_SIZE >> 1;
	int con_v_mem_size = v_mem_size / NR_CONSOLES;

	p_tty->p_console->origin_addr			= con_v_mem_size * nr_tty;
	p_tty->p_console->mem_limit				= con_v_mem_size;
	p_tty->p_console->current_start_addr	= p_tty->p_console->origin_addr;
	p_tty->p_console->cursor				= p_tty->p_console->origin_addr;

	if(nr_tty == 0)
	{
		p_tty->p_console->cursor = disp_pos >> 1;
		disp_pos = 0;
	}else
	{
		out_char(p_tty->p_console,nr_tty + '1');
		out_char(p_tty->p_console,'#');
		out_char(p_tty->p_console,' ');
		out_char(p_tty->p_console,':');
	}

	set_cursor(p_tty->p_console->cursor);
}

PUBLIC void select_console(int nr_console)
{
	if(nr_console < 0 || nr_console >= NR_CONSOLES)
		return;
	nr_current_console = nr_console;

	flush(&console_table[nr_console]);
}

PUBLIC void scroll_screen(CONSOLE *p_con,int direct)
{
	switch(direct)
	{
	case SCR_UP:
		{
			if(p_con->current_start_addr  > p_con->origin_addr)
			{
				if(p_con->current_start_addr + (SCREEN_SIZE*1.5) > p_con->origin_addr + p_con->mem_limit)
				{
					p_con->current_start_addr -= SCREEN_SIZE >> 1;
				}
				else
				{
					p_con->current_start_addr -= SCREEN_SIZE;
				}
			}
			break;
		}
	case SCR_DN:
		{
			if(p_con->current_start_addr + (SCREEN_SIZE << 1) < p_con->origin_addr + p_con->mem_limit)
			{
				p_con->current_start_addr += SCREEN_SIZE;
			}
			else if(p_con->current_start_addr + (SCREEN_SIZE*1.5) < p_con->origin_addr + p_con->mem_limit)
			{
				p_con->current_start_addr += SCREEN_SIZE >> 1;
			}
			break;
		}
	case LIN_UP:
		{
			if(p_con->cursor > p_con->origin_addr)
				p_con->current_start_addr -= SCREEN_WIDTH;
			break;
		}
	case LIN_DN:
		{
			if(p_con->cursor < p_con->origin_addr + p_con->mem_limit - SCREEN_WIDTH)
				p_con->current_start_addr += SCREEN_WIDTH;
			break;
		}
	default:
		break;
	}
	flush(p_con);
}

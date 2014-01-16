/// \file console.h
/*
  -----------------------------------
  Create date: 2013-12-01 18:42
  Modified date: 2014-01-10 01:01
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

typedef struct s_console
{
	u32	current_start_addr;
	u32	origin_addr;
	u32 mem_limit;
	u32 cursor;
}CONSOLE;

#define SCR_UP	0
#define SCR_DN	1
#define LIN_UP	2
#define LIN_DN	3

#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH	80

#define DEFAULT_CHAR_COLOR	(MAKE_COLOR(BLACK, WHITE))
#define RED_CHAR			(MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define GRAY_CHAR			(MAKE_COLOR(BLUE,  RED) | BRIGHT)

#endif

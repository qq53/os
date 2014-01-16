/// \file tty.h
/*
  -----------------------------------
  Create date: 2013-12-01 18:39
  Modified date: 2013-12-01 19:19
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#ifndef _TTY_H_
#define _TTY_H_

#define TTY_IN_BYTES	256

struct s_console;

typedef struct s_tty
{
	u32	in_buf[TTY_IN_BYTES];
	int head;
	int tail;
	int count;

	struct s_console* p_console;
}TTY;

#endif

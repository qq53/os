/// \file type.h
/*
  -----------------------------------
  Create date: 2013-11-26 19:27
  Modified date: 2014-01-09 23:39
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#ifndef	_TYPE_H_
#define	_TYPE_H_

typedef	unsigned int	u32;
typedef	unsigned short	u16;
typedef	unsigned char	u8;

typedef	void (*int_handler) ();

typedef void (*task_call) (u32);

typedef void (*irq_handler) (u32);

typedef void* system_call;

typedef char* va_list;

typedef struct
{
	int source;
	int type;
	union
	{
		int mess1;
	}u;
}MESSAGE;

#endif

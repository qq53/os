/// \file systask.c
/*
  ------------------------------------
  Create date : 2014-01-09 23:36
  Modified date: 2014-01-15 16:55
  Author : Sen1993
  Email : 1730806439@qq.com
  ------------------------------------
*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PUBLIC void task_sys()
{
	MESSAGE msg;
	
	while(1)
	{
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;
		switch(msg.type)
		{
		case GET_TICKS:
			msg.u.mess1 = ticks;
			send_recv(SEND, src, &msg);
			break;
		default:
			panic("unknown msg type");
			break;
		}
	}
}

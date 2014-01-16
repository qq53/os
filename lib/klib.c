/// \file klib.c
/*
  -----------------------------------
  Create date: 2013-11-23 23:36
  Modified date: 2014-01-08 15:43
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "type.h"
#include "const.h"
#include "string.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC char* itoa(char* str,int num)
{
	char *p = str;
	char ch;
	int i;
	int flag = 0;

	*p++ = '0';
	*p++ = 'X';

	if(num == 0)
	{
		*p++ = '0';
	}
	else
	{
		for(i=28;i>=0;i-=4)
		{
			ch = (num >> i) & 0xF;
			if(flag || ch > 0)
			{
				flag = 1;
				ch += '0';
				if(ch > '9')
					ch += 7;
				*p++ = ch;
			}
		}
	}
	*p = '\0';

	return str;
}

PUBLIC void delay(int time)
{
	int i, j, k;
	for(k=0;k<time;k++)
	{
		for(i=0;i<10000;i++)
		{
			for(j=0;j<10000;j++)
			{
			}
		}
	}
}

PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output,input);
	disp_str(output);
}

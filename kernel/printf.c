/// \file printf.c
/*
  -----------------------------------
  Create date: 2013-12-04 01:18
  Modified date: 2014-01-12 01:16
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "const.h"
#include "type.h"

int printf(const char *fmt, ...)
{
	int i;
	char buf[256];

	va_list arg = (va_list)((char*)(&fmt) + 4);
	i = vsprintf(buf, fmt, arg);
	buf[i] = 0;
	printx(buf);

	return i;
}

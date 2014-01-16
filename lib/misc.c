/// \file misc.c
/*
  ------------------------------------
  Create date : 2014-01-08 15:27
  Modified date: 2014-01-08 17:20
  Author : Sen1993
  Email : 1730806439@qq.com
  ------------------------------------
*/
#include "type.h"
#include "const.h"
#include "string.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PUBLIC void spin(char *func_name)
{
	printl("\nspinning in %s ... \n", func_name);
	while(1) {}
}

PUBLIC void assertion_failure(char *exp,char *file,char *base_file,int line)
{
	printl("%c assert(%s) failed: file: %s base_file: %s line: %d",
			MAG_CH_ASSERT,
			exp,file,base_file,line);

	spin("assertion_failure()");

	__asm__ __volatile__("ud2");
}

/// \file start.c
/*
  -----------------------------------
  Create date: 2013-11-23 22:58
  Modified date: 2013-12-03 21:34
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC void cstart()
{
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n");

	///初始化GDT表
	memcpy(&gdt,(void*)(*((u32*)&(gdt_ptr[2]))),*((u16*)(&gdt_ptr[0]))+1);
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base = (u32)&gdt;

	///初始化IDT表
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base = (u32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_idt_base = (u32)&idt;

	init_prot();
}

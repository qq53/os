/// \file const.h
/*
  -----------------------------------
  Create date: 2013-11-23 23:46
  Modified date: 2014-01-10 01:10
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#ifndef _CONST_H_
#define _CONST_H_

#define EXTERN extern

#define PUBLIC
#define PRIVATE	static

#define TRUE	1
#define FALSE	0
#define NULL	0

#define BLACK   0x0						/* 0000 */
#define WHITE   0x7						/* 0111 */
#define RED     0x4						/* 0100 */
#define GREEN   0x2						/* 0010 */
#define BLUE    0x1						/* 0001 */
#define FLASH   0x80					/* 1000 0000 */
#define BRIGHT  0x08					/* 0000 1000 */
#define	MAKE_COLOR(x,y)	((x<<4) | y)	/* MAKE_COLOR(Background,Foreground) */

#define GDT_SIZE 128
#define IDT_SIZE 256

#define STR_DEFAULT_LEN 1024

#define RPL_KRNL	SA_RPL0
#define RPL_TASK	SA_RPL1
#define RPL_USER	SA_RPL3

#define	PRIVILEGE_KRNL	RPL_KRNL
#define	PRIVILEGE_TASK	RPL_TASK
#define	PRIVILEGE_USER	RPL_USER

#define INT_M_CTL		0x20
#define INT_M_CTLMASK	0x21
#define	INT_S_CTL		0xA0
#define INT_S_CTLMASK	0xA1

#define NR_IRQ			16

#define CLOCK_IRQ		0
#define KEYBOARD_IRQ	1

#define	NR_SYS_CALL		2

/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40		/* I/O port for timer channel 0 */
#define TIMER_MODE     0x43		/* I/O port for timer mode control */
#define RATE_GENERATOR 0x34		/* 00-11-010-0 :
								* Counter0 - LSB then MSB - rate generator - binary*/
#define TIMER_FREQ     1193182L /* clock frequency for timer in PC and AT */
#define HZ             1000		/* clock freq (soassftware settable on IBM-PC) */

#define KB_DATA			0x60
#define KB_CMD			0x64

#define CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	START_ADDR_H	0xC		/* reg index of video mem start addr (MSB) */
#define	START_ADDR_L	0xD		/* reg index of video mem start addr (LSB) */
#define	CURSOR_H		0xE		/* reg index of cursor position (MSB) */
#define	CURSOR_L		0xF		/* reg index of cursor position (LSB) */
#define	V_MEM_BASE		0xB8000	/* base of color video memory */
#define	V_MEM_SIZE		0x8000	/* 32K: B8000H -> BFFFFH */

#define	NR_CONSOLES		3		/* 32K显存使用3个控制台 */

#define	LED_CODE		0xED
#define KB_ACK			0xFA

#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp,char *file,char *base_file,int line);
#define assert(exp) if(exp); \
		else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

#define ANY		(NR_TASKS + 8)
#define NO_TASK	(NR_TASKS + 16)

#define INTERRUPT		-10
#define INVALID_DRIVER	-20

#define SEND	1
#define RECEIVE 2
#define BOTH	3

#define ABLED		1
#define SENDING		2
#define RECEIVING	4//1bit is use to decide the process whether to run

enum msgtype
{
	HARD_INT = 1,
	GET_TICKS
};

#define TASK_TTY	0
#define TASK_SYS	1

#endif

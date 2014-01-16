/// \file keyboard.c
/*
  -----------------------------------
  Create date: 2013-11-30 16:41
  Modified date: 2013-12-05 17:13
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
#include "type.h"
#include "const.h"
#include "string.h"
#include "protect.h"
#include "proc.h"
#include "keyboard.h"
#include "keymap.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PRIVATE KB_INPUT	kb_in;
PRIVATE	int			code_with_E0;
PRIVATE	int			shift_l;	/* l shift state */
PRIVATE	int			shift_r;	/* r shift state */
PRIVATE	int			alt_l;		/* l alt state	 */
PRIVATE	int			alt_r;		/* r left state	 */
PRIVATE	int			ctrl_l;		/* l ctrl state	 */
PRIVATE	int			ctrl_r;		/* l ctrl state	 */
PRIVATE	int			caps_lock;	/* Caps Lock	 */
PRIVATE	int			num_lock;	/* Num Lock	 */
PRIVATE	int			scroll_lock;	/* Scroll Lock	 */
PRIVATE	int			column;

PRIVATE	u8			get_byte_from_kbuf();
PRIVATE void		set_leds();

PUBLIC void keyboard_handler(u32 param)
{
	u8 scan_code = in_byte(KB_DATA);
	
	if(kb_in.head >= KB_IN_BYTES - 1)
		kb_in.head = -1;
	kb_in.buf[++kb_in.head] = scan_code;
	++kb_in.count;
}

PUBLIC void init_keyboard()
{
	kb_in.head = kb_in.tail = -1;
	kb_in.count = 0;

	shift_l	= shift_r = 0;
	alt_l	= alt_r   = 0;
	ctrl_l	= ctrl_r  = 0;

	caps_lock = scroll_lock = 0;
	num_lock = 1;

	set_leds();

	put_irq_handler(KEYBOARD_IRQ,keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}

PUBLIC void keyboard_read(TTY* p_tty)
{
	u8 scan_code;
	int make;

	u32 key = 0;
	u32* keyrow;

	if(kb_in.count > 0)
	{
		code_with_E0 = 0;
		scan_code = get_byte_from_kbuf();

		if(scan_code == 0xE1)
		{
			int i;
			u8 pausebrk_scode[] = {0xE1,0x1D,0x45,0xE1,0x9D,0xC5};
			
			for(i=1;i<6;++i)
			{
				if(get_byte_from_kbuf() != pausebrk_scode[i])
					break;
			}
			if(i == 6)
				key = PAUSEBREAK;
		}
		else if (scan_code == 0xE0)
		{
			scan_code = get_byte_from_kbuf();

			if(scan_code == 0x2A && get_byte_from_kbuf() == 0xE0 && get_byte_from_kbuf() == 0x37)
			{
				key	= PRINTSCREEN;
				make = 1;
			}
			else if(scan_code == 0xB7 && get_byte_from_kbuf() == 0xE0 && get_byte_from_kbuf() == 0xAA)
			{
				key = PRINTSCREEN;
				make = 0;
			}
			if(key == 0)
				code_with_E0 = 1;
		}
		if(key != PAUSEBREAK && key != PRINTSCREEN)
		{
			make	= scan_code & FLAG_BREAK ? FALSE : TRUE;
			keyrow	= &keymap[(scan_code & 0x7F) * MAP_COLS];
			column	= 0;

			int caps = shift_l || shift_r;

			if(caps_lock)
			{
				if(keyrow[0] >= 'a' && keyrow[0] <= 'z')
					caps = !caps;
			}
			if(caps)
				column = 1;
			if(code_with_E0)
				column = 2;

			key = keyrow[column];
			switch(key)
			{
			case SHIFT_L:
				shift_l = make;
				break;
			case SHIFT_R:
				shift_r = make;
				break;
			case CTRL_L:
				ctrl_l = make;
				break;
			case CTRL_R:
				ctrl_r = make;
				break;
			case ALT_L:
				alt_l = make;
				break;
			case ALT_R:
				alt_l = make;
				break;
			case CAPS_LOCK:
				if(make)
				{
					caps_lock = !caps_lock;
					set_leds();
				}
				break;
			case NUM_LOCK:
				if(make)
				{
					num_lock = !num_lock;
					set_leds();
				}
				break;
			case SCROLL_LOCK:
				if(make)
				{
					scroll_lock = !scroll_lock;
					set_leds();
				}
				break;
			default:
				break;
			}
			if(make)///忽略Break Code
			{
				int pad = 0;

				/* 首先处理小键盘 */
				if (key >= PAD_SLASH && key <= PAD_9) 
				{
					pad = 1;
					switch(key) 
					{
					case PAD_SLASH:
						key = '/';
						break;
					case PAD_STAR:
						key = '*';
						break;
					case PAD_MINUS:
						key = '-';
						break;
					case PAD_PLUS:
						key = '+';
						break;
					case PAD_ENTER:
						key = ENTER;
						break;
					default:
						if (num_lock && key >= PAD_0 && key <= PAD_9) 
							key = key - PAD_0 + '0';
						else if (num_lock && key == PAD_DOT) 
							key = '.';
						else
						{
							switch(key) 
							{
							case PAD_HOME:
								key = HOME;
								break;
							case PAD_END:
								key = END;
								break;
							case PAD_PAGEUP:
								key = PAGEUP;
								break;
							case PAD_PAGEDOWN:
								key = PAGEDOWN;
								break;
							case PAD_INS:
								key = INSERT;
								break;
							case PAD_UP:
								key = UP;
								break;
							case PAD_DOWN:
								key = DOWN;
								break;
							case PAD_LEFT:
								key = LEFT;
								break;
							case PAD_RIGHT:
								key = RIGHT;
								break;
							case PAD_DOT:
								key = DELETE;
								break;
							default:
								break;
							}
						}
						break;
					}
				}

				key |= shift_l	? FLAG_SHIFT_L	: 0;
				key |= shift_r	? FLAG_SHIFT_R	: 0;
				key |= ctrl_l	? FLAG_CTRL_L	: 0;
				key |= ctrl_r	? FLAG_CTRL_R	: 0;
				key |= alt_l	? FLAG_ALT_L	: 0;
				key |= alt_r	? FLAG_ALT_R	: 0;
				key |= pad		? FLAG_PAD		: 0;

				in_process(p_tty,key);
			}
		}
	}
}

u8 get_byte_from_kbuf()
{
	u8 scan_code;

	while(kb_in.count <= 0){}
	if(kb_in.tail >= KB_IN_BYTES - 1)
		kb_in.tail = -1;

	disable_int();
	scan_code = kb_in.buf[++kb_in.tail];
	--kb_in.count;
	enable_int();

	return scan_code;
}

PRIVATE void kb_wait()
{
	while(in_byte(KB_CMD) & 0x2);
}

PRIVATE void kb_ack()
{
	while(in_byte(KB_DATA) != KB_ACK);
}

PRIVATE void set_leds()
{
	u8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

	kb_wait();
	out_byte(KB_DATA,LED_CODE);
	kb_ack();

	kb_wait();
	out_byte(KB_DATA,leds);
	kb_ack();
}

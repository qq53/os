/// \file proc.c
/*
  ------------------------------------
  Create date : 2014-01-15 22:18
  Modified date: 2014-01-15 22:56
  Author : Sen1993
  Email : 1730806439@qq.com
  ------------------------------------
*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PRIVATE void block(PROCESS* p);
PRIVATE void unblock(PROCESS* p);
PRIVATE int  msg_send(PROCESS* current, int dest, MESSAGE* m);
PRIVATE int  msg_receive(PROCESS* current, int src, MESSAGE* m);
PRIVATE int  deadlock(int src, int dest);

PUBLIC void schedule()
{
	PROCESS*	p;
	int	greatest_ticks = 0;

	while (!greatest_ticks) 
	{
		for (p = proc_table; p < proc_table + NR_TASKS; ++p) 
		{
			if (p->p_flags == 0 && p->ticks > greatest_ticks) 
			{
				greatest_ticks = p->ticks;
				if(procpri(p) != procpri(p_proc_ready))
				{
					if(procpri(p) == PRIVILEGE_TASK)
						ldt_sel = SELECTOR_LDT_TASK;
					else
						ldt_sel = SELECTOR_LDT_USER;
				}
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks)
			for (p = proc_table; p < proc_table + NR_TASKS; ++p)
				if (p->p_flags == 0)
					p->ticks = p->priority;
	}
}

///The core routine of system call 'sendrec()'
PUBLIC int sys_sendrec(int function, int src_dest, MESSAGE* m, PROCESS* p)
{
	assert(k_reenter == 0);	/// make sure we are not in ring0
	assert((src_dest >= 0 && src_dest < NR_TASKS) ||
	        src_dest == ANY ||
	        src_dest == INTERRUPT);

	int ret = 0;
	
	m->source = proc2pid(p);

	assert(m->source != src_dest);

	if (function == SEND) 
	{
		ret = msg_send(p, src_dest, m);
		if (ret != 0)
			return ret;
	}
	else if (function == RECEIVE) 
	{
		ret = msg_receive(p, src_dest, m);
		if (ret != 0)
			return ret;
	}
	else 
	{
		panic("<sys_sendrec> invalid function: %d (SEND:%d, RECEIVE:%d)\n", function, SEND, RECEIVE);
	}

	return 0;
}

/// <Ring 1~3> IPC syscall
PUBLIC int send_recv(int function, int src_dest, MESSAGE* msg)
{
	int ret = 0;

	if (function == RECEIVE)
		memset(msg, 0, sizeof(MESSAGE));

	switch (function) 
	{
	case BOTH:
		ret = sendrec(SEND, src_dest, msg);
		if (ret == 0)
			ret = sendrec(RECEIVE, src_dest, msg);
		break;
	case SEND:
	case RECEIVE:
		ret = sendrec(function, src_dest, msg);
		break;
	default:
		assert(function == BOTH || function == SEND || function == RECEIVE);
		break;
	}

	return ret;
}

/// <Ring 0~3> Clear up a MESSAGE by setting each byte to 0
PUBLIC void reset_msg(MESSAGE* p)
{
	memset(p, 0, sizeof(MESSAGE));
}

/** <Ring 0> This routine is called after 'p_flags' has been set (!= 0), it
	 calls 'schedule()' to choose another proc */
PRIVATE void block(PROCESS* p)
{
	assert(p->p_flags);
	schedule();
}

/** <Ring 0> This is a dummy routine. It does nothing actually. When it is
	called, the 'p_flags' should have been cleared */
PRIVATE void unblock(PROCESS* p)
{
	assert(p->p_flags == 0);
}

/// make sure not have a message circle
PRIVATE int deadlock(int src, int dest)
{
	PROCESS* p = proc_table + dest;
	
	while (1) 
	{
		if (p->p_flags & SENDING) 
		{
			if (p->p_sendto == src) 
			{
				p = proc_table + dest;
				printl("=_=%s", p->p_name);
				do 
				{
					assert(p->p_msg);
					p = proc_table + p->p_sendto;
					printl("->%s", p->p_name);
				} while (p != proc_table + src);
				printl("=_=");

				return 1;
			}
			p = proc_table + p->p_sendto;
		}
		else 
			break;
	}

	return 0;
}

/** <Ring 0> Send a message to the dest proc. If dest is blocked waiting for
	the message, copy the message to it and unblock dest. Otherwise the caller
	will be blocked and appended to the dest's sending queue */
PRIVATE int msg_send(PROCESS* current, int dest, MESSAGE* m)
{
	PROCESS* sender = current;
	PROCESS* p_dest = proc_table + dest;

	assert(proc2pid(sender) != dest);

	if (deadlock(proc2pid(sender), dest)) 
	{
		panic(">>DEADLOCK<< %s->%s", sender->p_name, p_dest->p_name);
	}

	if ((p_dest->p_flags & RECEIVING) && /// dest is waiting for the msg
	    (p_dest->p_recvfrom == proc2pid(sender) ||
	     p_dest->p_recvfrom == ANY)) 
	{
		assert(p_dest->p_msg);
		assert(m);

		phys_copy(p_dest->p_msg, m,sizeof(MESSAGE));

		p_dest->p_msg = NULL;
		p_dest->p_flags &= ~RECEIVING; /// dest has received the msg
		p_dest->p_recvfrom = NO_TASK;
		unblock(p_dest);

		assert(p_dest->p_flags == 0);
		assert(p_dest->p_msg == NULL);
		assert(p_dest->p_recvfrom == NO_TASK);
		assert(p_dest->p_sendto == NO_TASK);
		assert(sender->p_flags == 0);
		assert(sender->p_msg == NULL);
		assert(sender->p_recvfrom == NO_TASK);
		assert(sender->p_sendto == NO_TASK);
	}
	else
	{ 
		/// dest is not waiting for the msg
		sender->p_flags |= SENDING;

		assert(sender->p_flags == SENDING);
		
		sender->p_sendto = dest;
		sender->p_msg = m;

		/// append to the sending queue
		PROCESS* p;

		if (p_dest->q_sending) 
		{
			p = p_dest->q_sending;
			while (p->next_sending)
				p = p->next_sending;
			p->next_sending = sender;
		}
		else
			p_dest->q_sending = sender;
		
		sender->next_sending = NULL;

		block(sender);

		assert(sender->p_flags == SENDING);
		assert(sender->p_msg != NULL);
		assert(sender->p_recvfrom == NO_TASK);
		assert(sender->p_sendto == dest);
	}

	return 0;
}

/** <Ring 0> Try to get a message from the src proc. If src is blocked sending
	the message, copy the message from it and unblock src. Otherwise the caller
	will be blocked */
PRIVATE int msg_receive(PROCESS* current, int src, MESSAGE* m)
{
	PROCESS* p_recv = current;
	PROCESS* p_from = 0;
	PROCESS* prev = 0;
	int copyok = FALSE;

	assert(proc2pid(p_recv) != src);

	if ((p_recv->has_int_msg) &&
	    ((src == ANY) || (src == INTERRUPT)))
	{
		/// There is an interrupt to handle it

		MESSAGE msg;

		reset_msg(&msg);
		msg.source = INTERRUPT;
		msg.type = HARD_INT;

		assert(m);

		phys_copy(m, &msg, sizeof(MESSAGE));

		p_recv->has_int_msg = FALSE;

		assert(p_recv->p_flags == 0);
		assert(p_recv->p_msg == NULL);
		assert(p_recv->p_sendto == NO_TASK);
		assert(p_recv->has_int_msg == FALSE);

		return 0;
	}

	/// Arrives here if no interrupt for p_recv
	if (src == ANY) 
	{
		/** p_recv is ready to receive messages from
		    ANY proc, we'll check the sending queue and pick the
		    first proc in it */
		if (p_recv->q_sending)
		{
			p_from = p_recv->q_sending;
			copyok = TRUE;

			assert(p_recv->p_flags == 0);
			assert(p_recv->p_msg == NULL);
			assert(p_recv->p_recvfrom == NO_TASK);
			assert(p_recv->p_sendto == NO_TASK);
			assert(p_recv->q_sending != NULL);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != NULL);
			assert(p_from->p_recvfrom == NO_TASK);
			assert(p_from->p_sendto == proc2pid(p_recv));
		}
	}
	else
	{
		/** p_recv wants to receive a message from
		    a certain proc: src */
		p_from = &proc_table[src];

		if ((p_from->p_flags & SENDING) &&
		    (p_from->p_sendto == proc2pid(p_recv)))
		{
			copyok = TRUE;

			PROCESS* p = p_recv->q_sending;
			assert(p);
			
			while (p) 
			{
				assert(p_from->p_flags & SENDING);
				
				if (proc2pid(p) == src) 
				{
					p_from = p;
					break;
				}
				prev = p;
				p = p->next_sending;
			}

			assert(p_recv->p_flags == 0);
			assert(p_recv->p_msg == NULL);
			assert(p_recv->p_recvfrom == NO_TASK);
			assert(p_recv->p_sendto == NO_TASK);
			assert(p_recv->q_sending != NULL);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != NULL);
			assert(p_from->p_recvfrom == NO_TASK);
			assert(p_from->p_sendto == proc2pid(p_recv));
		}
	}

	if (copyok) 
	{
		/** It's determined from which proc the message will
		    be copied. Note that this proc must have been
		    waiting for this moment in the queue, so we should
		    remove it from the queue */
		if (p_from == p_recv->q_sending) 
		{
			assert(prev == NULL);

			p_recv->q_sending = p_from->next_sending;
		}
		else 
		{
			assert(prev);

			prev->next_sending = p_from->next_sending;
		}
		p_from->next_sending = NULL;

		assert(m);
		assert(p_from->p_msg);
		
		phys_copy(m, p_from->p_msg, sizeof(MESSAGE));

		p_from->p_msg = NULL;
		p_from->p_sendto = NO_TASK;
		p_from->p_flags &= ~SENDING;
		unblock(p_from);
	}
	else
	{  /** nobody's sending any msg
		   Set p_flags so that p_recv will not
		   be scheduled until it is unblocked */
		p_recv->p_flags |= RECEIVING;

		p_recv->p_msg = m;

		if (src == ANY)
			p_recv->p_recvfrom = ANY;
		else
			p_recv->p_recvfrom = proc2pid(p_from);

		block(p_recv);

		assert(p_recv->p_flags == RECEIVING);
		assert(p_recv->p_msg != NULL);
		assert(p_recv->p_recvfrom != NO_TASK);
		assert(p_recv->p_sendto == NO_TASK);
		assert(p_recv->has_int_msg == FALSE);
	}

	return 0;
}

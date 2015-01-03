---
layout: post
date: 2014-12-28
title: Kernel siganl
category: kernels
---

#Data struct
=struct signal_sturct: 
The "struct signal_struct" is the random *leftovers* from all the other stuff.
http://thread.gmane.org/gmane.linux.kernel/512831/focus=513990
=sighand_struct:

=sigpending
Store blocked signal info


#Workflow
=Generate signal
__send_signal();

=Process siganl
* SIGKILL (may be some other)
process in _send_signal()-> complete_signal() tsk->state |= TASK_WAKEKILL 
http://lwn.net/Articles/288056/
http://www.ibm.com/developerworks/library/l-task-killable/


* others
each time a switch is made from kernel mode to user mode, 
arch-specific: entry.S -> do_siganl()
{ 
	get_signal_deliver()
	{
		if fatal -> do_greoup_exit()->...__cleanup_sighand()
	}

	handle_signal() -> k->u(hanle)-sigreturn->k
}



#FAQ
=Non-mask signal
SIGKILL, SIGSTOP

=__schedule()

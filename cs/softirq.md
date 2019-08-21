---
tags: [ cs ] 
title: Softirq of Linux Kernel
date: 2017-04-03T13:09:05+08:00 
category: cs
---

# The old bottom half
ULK 1st: 4.6.6 Bottom Half
History: commit ad09492558ffa7c67f2b58d23d04dce9ffb9b9dd (tag: 0.99)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:09:07 2007 -0500
    [PATCH] Linux-0.99 (December 13, 1992)
Firo: There isn't to much useful comment. But the code is very simple. Search bh_base.
# task queue
history: commit 98606bddf430f0a60d21fba93806f4e3c736b170 (tag: 1.1.13)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:09:30 2007 -0500
    Import 1.1.13
+ * New proposed "bottom half" handlers:
+ * (C) 1994 Kai Petzke, wpp@marie.physik.tu-berlin.de
+ * Advantages:
+ * - Bottom halfs are implemented as a linked list.  You can have as many
+ *   of them, as you want.
+ * - No more scanning of a bit field is required upon call of a bottom half.
+ * - Support for chained bottom half lists.  The run_task_queue() function can be
+ *   used as a bottom half handler.  This is for example usefull for bottom
+ *   halfs, which want to be delayed until the next clock tick.
+ * Problems:
+ * - The queue_task_irq() inline function is only atomic with respect to itself.
+ *   Problems can occur, when queue_task_irq() is called from a normal system
+ *   call, and an interrupt comes in.  No problems occur, when queue_task_irq()
+ *   is called from an interrupt or bottom half, and interrupted, as run_task_queue()
+ *   will not be executed/continued before the last interrupt returns.  If in
+ *   doubt, use queue_task(), not queue_task_irq().
+ * - Bottom halfs are called in the reverse order that they were linked into
+ *   the list.
+struct tq_struct {
Check ULK2nd 4.7.3.1 Extending a bottom half for task queues, especially tq_context and keventd
The Old Task Queue Mechanism in LKD3rd. Cition from it below.
[The end of task queues](https://lwn.net/Articles/11351/)

# Softirq
[I’ll Do It Later: Softirqs, Tasklets, Bottom Halves, Task Queues, Work Queues and Timers](http://www.cs.unca.edu/brock/classes/Spring2013/csci331/notes/paper-1130.pdf)
* not allow execute nest but can recusive lock:local_bh_disable 
current->preemt_count + SOFIRQ_OFFSET also disable preempt current process.
* hardirq on, can't sleep
* not percpu

# Occassions of Softirq
irq_exit()
re-enables softirq, local_bh_enable/spin_unlock_bh(); explicity checks executes, netstack/blockIO.
ksoftirqd

# Tasklet
History: commit 6cc120a8e71a8d124bf6411fc6e730a884b82701 (tag: 2.3.43pre7)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:30:52 2007 -0500
    Import 2.3.43pre7
+ Tasklets --- multithreaded analogue of BHs.
+   Main feature differing them of generic softirqs: tasklet
+   is running only on one CPU simultaneously.
+   Main feature differing them of BHs: different tasklets
+   may be run simultaneously on different CPUs.
+   Properties:
+   * If tasklet_schedule() is called, then tasklet is guaranteed
+     to be executed on some cpu at least once after this.
+   * If the tasklet is already scheduled, but its excecution is still not
+     started, it will be executed only once.
+   * If this tasklet is already running on another CPU (or schedule is called
+     from tasklet itself), it is rescheduled for later.
+   * Tasklet is strictly serialized wrt itself, but not
+     wrt another tasklets. If client needs some intertask synchronization,
+     he makes it with spinlocks.

# Timer
## irqsafe timer
 __run_timers
irqsafe = timer->flags & TIMER_IRQSAFE
check del_timer_sync
and definition of TIMER_IRQSAFE
https://patchwork.kernel.org/patch/10811995/
Is timer pending


# LQO
[Deal PF_MEMALLOC in softirq](http://thread.gmane.org/gmane.linux.kernel/1152658)

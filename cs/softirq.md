---
tags: [ cs ] 
title: Softirq of Linux Kernel
date: 2017-04-03T13:09:05+08:00 
category: cs
---

# Softirq
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
+/* Tasklets --- multithreaded analogue of BHs.
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

# LQO
[Deal PF_MEMALLOC in softirq](http://thread.gmane.org/gmane.linux.kernel/1152658)

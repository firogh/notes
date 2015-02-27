---
tags: kernel
title: kernel-activites
date: 2015-02-27 15:46:12
category: kernel
---
#parts
=interrupt
irq
softirq, tasklet

=process 
kernel thread, workqueue
process(user & kernel mode)


#kthread and workqueue
kthread do specific func onshot
workqueue can dynamic set different specific work
workqueue is just a coat of kthread

#softirq
* not allow execute nest but can recusive lock:local_bh_disable 
current->preemt_count + SOFIRQ_OFFSET also disable preempt current process.
* hardirq on, can't sleep
* not percpu
* tasklet and kernel timer is based on softirq
* First of all, it's a conglomerate of mostly unrelated jobs, 
 which run in the context of a randomly chosen victim 
 w/o the ability to put any control on them. --Thomas Gleixner

#tasklet
tasklet like a workqueue, sofirq like kthread. that's wonderful, does it?

#What is context?
=process context: 
* register, kernel task_struct, stack. user text stack heap
=interrupt context: 
* Firo thinks the key point is interrupt context is irrelevant to process context!
* Can not schedule!
* register, maybe interrupt/process stack.

#what is interrupt context and softirq context?

#Why disable preempt in spinlock
or Deadlock

#spin_lock_bh

#softirq context
in the context of whichever process happens to be running at the wrong time; 
that is the "randomly chosen victim" aspect that Thomas was talking about.


#preempt_count
* preempt 8
* softirq 8
* irq	4
* preempt active 1
* nmi 1

#irq and lock
irq and lock are complete different things!
when need to disable irq just get rid of mess of shared data.

#time of preempt



#time of softirq
* follow hardirq, irq_exit()
* re-enables softirq, local_bh_enable/spin_unlock_bh(); explicity checks executes, netstack/blockIO.
* ksoftirqd


#When to save irq
Date: Sat, 12 Feb 2000 12:13:24 +0100 
From: Manfred Spraul <manfreds [at] colorfullife> 
AFAICS net_tx_action() is never called with disabled interrupts, so 
the local_irq_save() calls could be replaced with 
local_irq_disable(). I've attached an untested patch. 
Looks fine, patch applied. 
Later, 
David S. Miller 
davem [at] redhat 

local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

#why disable irq, by the way disable preempt of process.
Does it harm?


#what about irq nested?
http://lwn.net/Articles/380937/


#How to disable irq neset?




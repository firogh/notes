---
tags: kernel
title: Kernel activities
date: 2015-02-27 15:46:12
category: kernel
---

#Kernel activities
* Interrupt -- irq,softirq
* Process	-- user process, kernel thread(workqueue)

##What is context?
###process context: 
* register, kernel task_struct, stack. user text stack heap
###interrupt context: 
* Firo thinks the key point is interrupt context is irrelevant to process context!
* Can not schedule!
* register, maybe interrupt/process stack.
###softirq context
in the context of whichever process happens to be running at the wrong time; 
that is the "randomly chosen victim" aspect that Thomas was talking about.

##preempt_count
* preempt 8
* softirq 8
* irq	4
* preempt active 1
* nmi 1

#Interrupt
##Interrupt Context terminology
* in_irq
in __irq_enter I found preempt_count_add(HARDIRQ_OFFSET) only in here!
+irq disable 部设计preempt_cout

* in_softirq
should rename to in_softirq_or_bh_disable()
local_bh_disable use SOFTIRQ_DISABLE_OFFSET  (2 * SOFTIRQ_OFFSET)
__do_softirq use __local_bh_disable_ip(_RET_IP_, SOFTIRQ_OFFSET), damn it!

* in_interrupt
一种是irq路径上的do_softirq, 即in_serving_softirq
+local_bh_disbale()
//not in softirq context!!!
+local_bh_enable()
硬中断呢?
do_IRQ,肯定是.
local_irq_disable,不是, 但是no operation with preempt_count() 这种情况x86 in_interrupt是无感的.use irqs_disabled to check
[Question about in_interrupt() semantics with regard to softirqs]()

[Deal PF_MEMALLOC in softirq](http://thread.gmane.org/gmane.linux.kernel/1152658)

##softirq
* not allow execute nest but can recusive lock:local_bh_disable 
current->preemt_count + SOFIRQ_OFFSET also disable preempt current process.
* hardirq on, can't sleep
* not percpu
* tasklet and kernel timer is based on softirq
* First of all, it's a conglomerate of mostly unrelated jobs, 
 which run in the context of a randomly chosen victim 
 w/o the ability to put any control on them. --Thomas Gleixner

tasklet different with other softirq is run  signal cpu core
spinlock_bh wider then spinlock 

###time of softirq
* follow hardirq, irq_exit()
* re-enables softirq, local_bh_enable/spin_unlock_bh(); explicity checks executes, netstack/blockIO.
* ksoftirqd

###tasklet
tasklet like a workqueue, sofirq like kthread. that is wonderful, does it?

#Process
##kthread and workqueue
kthread do specific func onshot
workqueue can dynamic set different specific work
workqueue is just a coat of kthread

## Differences between softirq and workqueue
Softirq(tasklet)
init static:	DECLARE_TASKLET(), DECLARE_TASKLET_DISABLED()
init dynamic:	tasklet_init()
schedule:	tasklet_schedule()
exit:		tasklet_kill()
pros and cons:	execute in interrupt context, serialization, tasklet blonged to the cpu which tasklet_schedule() it, oneshot

Workqueue
init static:	DECLARE_WORK()
init dynamic:	kzalloc() INIT_WORK(), kzalloc() PREPARE_WORK()
schedule1:	create_singlethread_workqueue(), create_workqueue()  
		queue_work()
exit1:		flush_workqueue(), destroy_workqueue()
schedule2:	schedule_work()
exit2:		(none)
pros and cons:	execute in process context, workqueue has two type, driver can create workqueue or use system workqueue, oneshot

#FAQ
##When to save irq rather than just disable irq
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

##what about irq nested?
http://lwn.net/Articles/380937/

##irq and lock
irq and lock are complete different things!
when need to disable irq just get rid of mess of shared data.



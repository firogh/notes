---
tags: [ cs ] 
title: Softirq of Linux Kernel
date: 2017-04-03T13:09:05+08:00 
category: cs
---

##softirq
同一个softirq可以在不同的CPU上同时运行，softirq必须是可重入的。
* not allow execute nest but can recusive lock:local_bh_disable 
current->preemt_count + SOFIRQ_OFFSET also disable preempt current process.
* hardirq on, can't sleep
* not percpu
## tasklet and kernel timer is based on softirq
新增softirq, 是要重新编译内核的, 试试tasklet也不错.
.不允许两个两个相同类型的tasklet同时执行，即使在不同的处理器上
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
tasklet 被__tasklet_schedule到某个cpu的percu 变量tasklet_vec.tail上保证了
只有一个cpu执行同一时刻.

#FAQ
##When to save irq rather than just disable irq
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

##what about irq nested?
http://lwn.net/Articles/380937/

[Deal PF_MEMALLOC in softirq](http://thread.gmane.org/gmane.linux.kernel/1152658)

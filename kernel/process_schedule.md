---
layout: post
date: 2014-12-28
title: the process scheduling of linux kernel 
category: kernel
---

#Reference
codes: comments of __schedule()
Book: Linux kernel development 


#Common concepts
* The process scheduler decides which process runs, when, and for how long.
* Multitasking: cooperative and preemptive.
* Preemption:Involuntarily suspending a running process.
* Timeslice: a slice of cpu's time.
* Yielding: The act of a process voluntarily suspending itself.
* Cooperative multitasking: a process does not stop running until it voluntary decides to do so.
* Time sharing: is cooperative mutitasking, 
* the codes that yeild the cpu when timeslice is zero while ret from syscall is also belong to user process!
so if !CONFIG_PREEMPT* is cooperative!
* CFS: Pick the task with the smallest vruntime. That's it!

# Schdule point of preempt 
1 After Interrupt return,preempt_count_irq.
2 cond_resched
3 preempt_count

#Preemption
* Linux kernel user mode is always User preemption.
When returning to user-space from a system call.
When returning to user-space from an interrupt hander.
* Linux kernel kernel mode is coppertive when CONFIG_PREEMPT is not set.
If a task in the kernel explicitly calls schedule()
If a task in the kernel blocks (which results in a all to schedule())
* Linux kernel kernel mode is coppertive + preemptive when CONFIG_PREEMPT is set.
schedule + blcoks
When an interrupt handler exits, before returning to kernel-space.
When kernel code becomes preemptible again.

#TIF_NEED_RESCHED
* The situation of set task need resched
scheduler_tick()
try_to_wake_up() when a process that has a higher priority than *current* is awakened.
Other?

#What is the PREEMPT_ACTIVE
* This sets a flag bit in the preemption counter that has such a large value
	that it is never affected by the regular preemption counter increments

* It indicates to the schedule function that scheduling was not 
	invoked in the normal way but as a result of a kernel preemption.

* This ensures that the next task is selected as quickly as possible without 
	the hassle of deactivating the current one. If a high-priority task is waiting 
	to be scheduled, it will be picked by the scheduler class and will be allowed to run.


#FAQ
* swtich_to?



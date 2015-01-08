---
layout: post
date: 2014-12-28
title: the process scheduling of linux kernel 
category: kernel
---

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




#parts
=the periodic scheduler scheduler_tick: mark need resched
* vrtime
* TIF_NEED_RESCHED

=try_to_wake_up and wake_up_new_task: mark TIF_NEED_RESCHED

=the main scheduler schedule: initiate a resched

#what is the PREEMPT_ACTIVE

#The situtation of process scheduling
=non-prempt
Kernel -> user 
User -> irq -> User




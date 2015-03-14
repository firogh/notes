---
layout: post
date: 2014-12-28
title: the schedule of linux kernel 
category: kernel
---

#Process schedule
##Reference
codes: comments of __schedule()
Book: Linux kernel development 

##Common concepts
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

##TIF_NEED_RESCHED
* The situation of set task need resched
scheduler_tick()
try_to_wake_up() when a process that has a higher priority than *current* is awakened.
Other?

##FAQ
* swtich_to?



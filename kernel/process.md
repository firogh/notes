---
tags: kernel
title: process-management
date: 2015-02-27 15:46:12
category: kernel
---
# 进程的栈顶指针esp
一个进程有两个栈，用户态和内核态各一个。所以就有两个栈顶指针。这两个栈顶指针，同一时刻只能存在一个，因为进程要不处于用户态要不就是内核态，别无其他。esp既表示进程的用户栈，也表进程的内核栈，要看进程处于用户态还是内核态。当进程由用户态切换到内核态时，把用户态SS和esp指针保存在进程的内核栈上，再读去TSS段中进程的内核态指针付给ss和esp。从内核态返回时，弹出进程内核栈上用户态ss和esp值。那么TSS段中进程的内核态指针是从哪里来的呢？进程切换process switch也叫上下文切换context switch只发生在内核态，必须理解而不是记住。一个进程会用到许多cpu的寄存器比如esp，eip等等，他们被称作Hardware context硬件上下文，如果进程是培养皿中的一条幼虫，那么硬件上下问就是培养液了。不同的虫子需要不同的培养液，新的培养液进来，旧的培养液也需要保存，以备稍后使用。当进程切换时，我们要保存当前被切换出去进程的硬件上下文，以备将来切换回来使用。应该明白为什么进程切换也叫上下文切换了。怎么保存上下文呢？在进程的task_struct结构的成员变量thread_struct结构保存被切换出去进程的硬件上下文。我们所关心的换出的进程的esp指针也保存在thread_struct当中了。当然，对于被切换进来的进程我们用这个进程的thread_struct成员中的esp 值更新硬件的esp寄存器，也需要更新GDT中的TSS里的esp0,以备将来用来从用户态切换到内核态时，找到进程内核栈的栈顶指针。That's all。

#Process flag
PF_MEMALLOC [Kill PF_MEMALLOC abuse](http://thread.gmane.org/gmane.linux.kernel/914878)

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



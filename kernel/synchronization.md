---
tags: kernel
title: Synchronization
date: 2015-02-27 15:46:12
category: kernel
---

#Reference
A must read book[Unreliable Guide To Locking -- Rusty Russell](https://www.kernel.org/pub/linux/kernel/people/rusty/kernel-locking/index.html)
Perfbook

#Etymology
[Synchronize](http://www.etymonline.com/index.php?term=synchronize)
synchronization means be of the same time.

# Common usage
Protect from interruption by hardware interrupts:
        local_irq_disable(int irq) & local_irq_enable(int irq)
Protection from software interrupts:
        local_bh_disable(void) & local_bh_enable(void)
Protection from other CPUs:
        spin_lock(spinlock_t *) & spin_unlock(spinlock_t *)
Preemption by other user contexts:
        preempt_disable(void) & preempt_enable(void) 

# Common lock case
## Between user contexts
mutex and spinlock
我更倾向于把nf_register_sockopt替换成spinlock

## Between user context and BH
spinlock_bh
BH不允许睡眠, 只能用spinlock + disableBH来到进程和软中断的同步.

## Between user context and tasklet or timer
spinlock_bh

## Between Softirqs
### Same softirqs(not include tasklet and timer)
The same softirq can run on the other CPUs: you can use a per-CPU array for better performance.
You'll need to use spin_lock() and spin_unlock() for shared data.

### different softirqs
spin_lock

## Between Tasklets/Timers
### For same tasklet/timers
Nothing!Same tasklet is never run on two CPUs at once
### For different task/timers
If another tasklet/timer wants to share data with your tasklet or timer , 
you will both need to use spin_lock() and spin_unlock() calls. 
spin_lock_bh() is unnecessary here, as you are already in a tasklet, 
and none will be run on the same CPU.
## Between Hard IRQ and Softirqs/Tasklets
In softirq, spin_lock_irq: disable irq for local preemptable and spinlock for SMP
## Between Two Hard IRQ Handlers
spin_lock_irqsave

# Common concepts
a critical section is a piece of code that accesses a shared resource 
(data structure or device) that must not be concurrently accessed by more than one thread of execution.

This overlap, where the result depends on the relative timing of multiple tasks, is called a race condition. 

#Synchronization in computer science
##Process syncrhonization
It means "make it synchronous", something like coexistence.

Process synchronization refers to the idea that multiple processes are to join up or handshake at a certain point, 
in order to reach an agreement or commit to a certain sequence of action.

I think Process syncrhonization is more geneanl concepts stands of a set of actions. 
It can be classfiy in two kinds:
* Mutual exclusion -- only one excution routine in  critical section
Readers–writers problem
The Producer-Consumer Problem 
* Cooperative
TCP Handshake 
* Deadlock
Dining philosophers problem
* Starvation
* Prioirty inversion
* Busy waiting

#Mutual exclusion
## spinlock - unsleepable mutex
non-preemptable
The fundamental type is the spinlock
Spinlocks are very small and fast, and can be used anywhere.

spinlock保护的critical section执行时间短且不睡眠, 副产品就是避免了
context switch, 相对semaphore效率高.
显然对于little critical section 使用semaphore而不是spinlock,将带来性能
上的巨大损耗.
* rw_lock - r/w spinlock
## semaphore and mutex
it was implement based on spinlock.
it can have more than one holder at any time (the number decided at initialization time), 
although it is most commonly used as a single-holder lock (a mutex).
if you can not get a semaphore, your task will put itself on the wait queue, and be woken 
up the semaphore is released.
### sleepable
### preemptable

## Lockless
### RCU - low read-side overhead, beyond rw_lock
### Per-cpu

# RCU Read copy-update
RCU is a library for the Linux kernel that allows kernel subsystems to 
synchronize access to shared data in an efficient manner.
## requirements
(1) support for concurrent readers, even during updates;
(2) low computation and storage overhead; and 
(3) deterministic completion time.
##Why RCU
1 RCU supports concurrency between a single updater and multiple readers is the  essential of RCU!
2. dealcok immunity
##What's bad
less applicable to update-only workloads
##How rcu work
* Reader
RCU ensures that reads are coherent by maintaining multiple versions of objects and ensuring that they are not freed up until all pre-existing read-side critical sections complete.
* Copy updater
The basic idea behind RCU (read-copy update) is to split destructive
operations into two parts, one that prevents anyone from seeing the data
item being destroyed, and one that actually carries out the destruction.
A "grace period" must elapse between the two parts, and this grace period
must be long enough that any readers accessing the item being deleted have 
since dropped their references.
If any statement in a given RCU read-side critical section precedes a grace period, 
then all statements in that RCU read-side critical section must complete before that grace period ends.

A thread uses RCU synchronization by calling synchronize_rcu,
which guarantees not to return until all the RCU critical sections executing 
when synchronize_rcu was called have completed.
##FAQ
* Difference with primitive and atomic
* Grace period is synchroinze_rcu or a softirq justment
* __rcu sparse will warn you if you access that pointer without the services of one of the variants of rcu_dereference().

=Limitations of RCU-rt?
=In contrast to SRCU, preemptible RCU only permits blocking within primitives that are both subject to prikority inheritance and non-blocking in a non-CONFIG_PREEMPT kernel.
=No need of mb in preempt read-side?
=irq in rcu_sched_qs
=However, note that RCU read-side critical sections that begin after the beginning of a given grace period can and will extend beyond the end of that grace period.?
=RCU read-side critical sections must be permitted in NMI handlers as well as irq handlers?
=classic RCU will happily awaken each and every sleeping CPU at least once per grace period in some cases?
##Implemention
In process switch, marker a  flag that will not use old data.
For SMP, Softirq callback release old data; 
For UP, free after synchronize_rcu.
Softirq rcu_data,rcu_bh_data
### Classic RCU  - maybe RCU tiny
	http://www.rdrop.com/users/paulmck/RCU/whatisRCU.html
Classic RCU requires that read-side critical sections obey the same rules obeyed by the critical sections of pure spinlocks: blocking or sleeping of any sort is strictly prohibited.
CPUmask
### Preemtible RCU -
[The design of preemptible read-copy-update](http://lwn.net/Articles/253651/)
 a multi-stage grace-period detection algorithm
call callback when wailist[old] == 0; 
	block only in priority inheritance non-blocking in non-preemt kernel
* softirq
	two stages grace period
	wait queues
	rcu_process_callbacks
	rcu_flipctr: a per-CPU two-element
		[a]:tracks critical sections that started before the current gp stage.
		[b]:tracks new rcu read-side critical sections, namely those starting during the current gp stage.
	rcu_ctrlblk.completed & 0x1
	rcu_flip_flag: synchronize the start of each grace-period stage: once a given CPU has responded to its rcu_flip_flag, it must refrain from incrementing the old grace-period stage. 
	rcu_mb_flag: rcu_flipped  force each CPU to execute a memory barrier at the end of each grace-period stage.
				rcu_mb_needed by the CPU that detects that the sum of the old counters is zero
				 rcu_mb_done only by the corresponding CPU, and even then only after executing a memory barrier.
###Tree RCU for hundreds and thounds CPU cores
* Downside of classical RCU in update
The update-side primitives which determine when pre-existing read-side critical sections have finished, 
	were designed with only a few tens of CPUs in mind.
Their scalability is limited by a global lock that must be acquired by each CPU at least once during each grace period.
##How to use?
Developers can use RCU critical sections and RCU synchronization to 
build data structures that allow concurrent reading, even during updates.
[RCU Usage In the Linux Kernel: One Decade Later](http://www2.rdrop.com/users/paulmck/techreports/RCUUsage.2013.02.24a.pdf)
a replacement of rw-lock
a Restricted Reference-Counting Mechanism 
a Bulk Reference-Counting Mechanism
a Poor Man's Garbage Collector
a Way of Providing Existence Guarantees
a Way of Waiting for Things to Finish
#Memory barrier
##Refernce
http://en.wikipedia.org/wiki/Memory_barrier
Documentation/memory-barriers.txt
##Common
* When a program runs on a single-CPU machine, the hardware performs the necessary bookkeeping 
to ensure that the program executes as if all memory operations were performed in the order 
specified by the programmer (program order), so memory barriers are not necessary. 
However, when the memory is shared with multiple devices, such as other CPUs in a multiprocessor 
system, or memory mapped peripherals, out-of-order access may affect program behavior. 
For example, a second CPU may see memory changes made by the first CPU in a sequence which differs from program order.
* Compiler and cpu do the same optimization: reorder of instructions
##The Linux kernel has a variety of different barriers that act at different
levels:
  (*) Compiler barrier.
  (*) CPU memory barriers.
  (*) MMIO write barrier.
##ACCESS_ONCE
Does it work cast a variable to volatile?
http://stackoverflow.com/questions/22758232/why-is-access-once-so-complex
##For volatile
http://www.geeksforgeeks.org/understanding-volatile-qualifier-in-c/

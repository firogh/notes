---
tags: [ kernel ] 
title: Synchronization
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# HP
A program is synchro-
nized if all accesses to shared data are ordered by synchronization operations.
A
data reference is ordered by a synchronization operation if, in every possible
execution, a write of a variable by one processor and an access (either a read or a
write) of that variable by another processor are separated by a pair of synchroni-
zation operations, one executed after the write by the writing processor and one
executed before the access by the second processor???. Cases where variables may
be updated without ordering by synchronization are called data races because the
execution outcome depends on the relative speed of the processors, and, like
races in hardware design, the outcome is unpredictable, which leads to another
name for synchronized programs: data-race-free.


#Reference
Perfbook
[Unreliable Guide To Locking](https://www.kernel.org/pub/linux/kernel/people/rusty/kernel-locking/index.html)
[Scalability Techniques for Practical Synchronization Primitives](http://queue.acm.org/detail.cfm?id=2698990)
[test-and-set](http://web.cecs.pdx.edu/~walpole/class/cse513/slides/3.pdf)
[Synchronization Instructions](https://people.cs.clemson.edu/~mark/synch.html)

# Why we need synchronization
a critical section is a piece of code that accesses a shared resource 
(data structure or device) that must not be concurrently accessed by more than one thread of execution.

This overlap, where the result depends on the relative timing of multiple tasks, is called a race condition. 
# How to use synchronization mechanism
A must read book[Unreliable Guide To Locking -- Rusty Russell](https://www.kernel.org/pub/linux/kernel/people/rusty/kernel-locking/index.html)
## Common usage
Protect from interruption by hardware interrupts:
        local_irq_disable(int irq) & local_irq_enable(int irq)
Protection from software interrupts:
        local_bh_disable(void) & local_bh_enable(void)
Protection from other CPUs:
        spin_lock(spinlock_t *) & spin_unlock(spinlock_t *)
Preemption by other user contexts:
        preempt_disable(void) & preempt_enable(void) 
# What is synchronization in computer science 
[Synchronize](http://www.etymonline.com/index.php?term=synchronize)
synchronization means be of the same time.
It means "make it synchronous", something like coexistence.

Process synchronization refers to the idea that multiple processes are 
to join up or handshake at a certain point, in order to reach an 
agreement or commit to a certain sequence of action.
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

# Mutual exclusion
## spinlock - unsleepable mutex
[Linux 内核的排队自旋锁FIFO Ticket Spinlock](https://www.ibm.com/developerworks/cn/linux/l-cn-spinlock/)
spinlock保护的critical section执行时间短且不睡眠, 副产品就是避免了
context switch, 相对semaphore效率高.
显然对于little critical section 使用semaphore而不是spinlock,将带来性能
上的巨大损耗.
* rw_lock

## Mutex
Monitor lock before sleep.

## RCU -- lockless
Check The Journey to RCU for more details
# Cooperative
## semaphore 
it was implement based on spinlock.
it can have more than one holder at any time (the number decided at initialization time), 
although it is most commonly used as a single-holder lock (a mutex).
if you can not get a semaphore, your task will put itself on the wait queue, and be woken 
up the semaphore is released.
sleepable
preemptable

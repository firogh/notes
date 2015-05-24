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
sleepable
preemptable

## Lockless
### RCU - low read-side overhead, beyond rw_lock
Check The Journey to RCU for more details
### Per-cpu

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

---
tags: [ kernel ] 
title: Synchronization
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# ref
[Linus on recursive lock](https://yarchive.net/comp/linux/recursive_locks.html)
[Unreliable Guide To Locking](https://www.kernel.org/pub/linux/kernel/people/rusty/kernel-locking/index.html)
[How to Quantify Scalability](http://www.perfdynamics.com/Manifesto/USLscalability.html)

# HP
A program is synchronized if all accesses to shared data are ordered by synchronization operations.
A data reference is ordered by a synchronization operation if, in every possible
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

# ref
0xax
Waiman Long: Recent Locking Improvements in the Linux Kernel

# Memory barier

# Per-CPU

# Atomic
## x86 locked atomic operations:
v3a chapter 8
* Guaranteed atomic operations
* Bus locking, using the LOCK# signal and the LOCK instruction prefix
* ??Cache coherency protocols that ensure that atomic operations can be carried out on cached data structures(cache lock); this mechanism is present in the Pentium 4, Intel Xeon, and P6 family processors
## Uniprocessor
[all signle instructions is atomic, including cmpxchg](https://wiki.osdev.org/Atomic_operation#Single_processor-single_core_systems)
[Anything is atomic if you do it with interrupts disabled on a UP system](https://stackoverflow.com/a/39358907/1025001)
## SMP
cmpxchg isn't atomic; it needs LOCK prefix.
# Examples
x86 inc isn't atomic; check atomic_inc.c
## RMW
[Atomic vs. Non-Atomic Operations](http://preshing.com/20130618/atomic-vs-non-atomic-operations/)
## misalgined access
[AMD 7.3.2 Access Atomicity](https://support.amd.com/TechDocs/24593.pdf#page=224)
Also check Intel 8.1.1
[naturally-aligned  vs atomic vs cache](https://stackoverflow.com/questions/36624881/why-is-integer-assignment-on-a-naturally-aligned-variable-atomic-on-x86/36685056#36685056)
## Why CAS?
## Cache locking
v3a: 8.1.4
Effects of a LOCK Operation on Internal Processor Caches
## cmpxchg vs xchg on LOCK prefix implicit
[Check Linux SMP alternatives](https://stackoverflow.com/a/44273130/1025001)
## interrupt/exception vs non-atomic operation
v3a 6.6
To allow the restarting of program or task following the handling of an exception or an interrupt, all exceptions
(except aborts) are guaranteed to report exceptions on an instruction boundary. All interrupts are guaranteed to be
taken on an instruction boundary.
## Two processor writing to the same address
[What happens when two CPU cores require write access to the same memory address at the same exact moment?](https://www.quora.com/What-happens-when-two-CPU-cores-require-write-access-to-the-same-memory-address-at-the-same-exact-moment)
[When an interrupt occurs, what happens to the process that is currently getting executed?](https://www.quora.com/When-an-interrupt-occurs-what-happens-to-the-process-that-is-currently-getting-executed)

# Local Interrupt Disabling

# Disabling and Enabling Deferrable Functions

# Spinlock - unsleepable mutex
[MCS locks and qspinlocks](https://lwn.net/Articles/590243/)
[MCS locks on quora](https://www.quora.com/How-does-an-MCS-lock-work)
## Bit spinlock
include/linux/bit_spinlock.h

# Optimistic spin queue
[Cancellable MCS spinlock rework](https://lwn.net/Articles/604130/)
[An Overview of Kernel Lock Improvements](http://events17.linuxfoundation.org/sites/events/files/slides/linuxcon-2014-locking-final.pdf)

# Mutex
Monitor lock before sleep.
Faireness: queue, rt mutex

# RW Lock
## rwlock
include/linux/rwlock.h
_QW_WMASK
_QW_WAITING
queued_read_trylock
queued_read_lock

# Semaphore 
[Linus on semaphore](https://yarchive.net/comp/linux/semaphores.html)
 150    213  kernel/printk/printk.c <<down_console_sem>>
             down(&console_sem);\
 151    231  net/core/netpoll.c <<netpoll_poll_disable>>
             down(&ni->dev_lock);
it was implement based on spinlock.
it can have more than one holder at any time (the number decided at initialization time), 
although it is most commonly used as a single-holder lock (a mutex).
if you can not get a semaphore, your task will put itself on the wait queue, and be woken 
up the semaphore is released.
sleepable
preemptable
* xadd vs spinlock
## LQO
* %k1 %w1 in kernel up_write
https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#x86Operandmodifiers
https://stackoverflow.com/questions/34459803/in-gnu-c-inline-asm-whatre-the-modifiers-for-xmm-ymm-zmm-for-a-single-operand?rq=1

# RW semaphore
[rwsem: Support optimistic spinning](https://lwn.net/Articles/598577/)

# RCU -- lockless
Check The Journey to RCU for more details
# examples
list_add_tail_rcu(&p->tasks, &init_task.tasks); in copy_process
no rcu read block and unlock

# LQO
##When to save irq rather than just disable irq, recusive?
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

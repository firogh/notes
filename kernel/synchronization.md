---
tags: kernel
title: Synchronization
date: 2015-02-27 15:46:12
category: kernel
---

#Reference
Perfbook

#Etymology
[Synchronize](http://www.etymonline.com/index.php?term=synchronize)
synchronization means be of the same time.

#Synchronization in computer science
##Process syncrhonization
It means "make synchronous", something like coexistence.

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
a critical section is a piece of code that accesses a shared resource 
(data structure or device) that must not be concurrently accessed by more than one thread of execution.

* semaphore - mutlti-mutex

* mutex

* spinlock - unsleepable mutex

* rw_lock - r/w spinlock

* rcu - low read-side overhead, beyond rw_lock


#RCU - Read - copy update
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


##Class
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

### SRCU


### RCU-task


##How to use?
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

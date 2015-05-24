---
tags: kernel
title: The Journey to RCU
date: 2015-02-27 15:46:12
category: kernel
---
<iframe frameborder="no" border="0" marginwidth="0" marginheight="0" width=330 height=86 src="http://music.163.com/outchain/player?type=2&id=444737&auto=1&height=66"></iframe>

# RCU Read copy-update
RCU is a library for the Linux kernel that allows kernel subsystems to 
synchronize access to shared data in an efficient manner.
## Why RCU
[Read-Copy Update Mutual-Exclusion in Linux](http://lse.sourceforge.net/locking/rcu/rcupdate_doc.html)
[Read Copy Update HOWTO](http://lse.sourceforge.net/locking/rcu/HOWTO/index.html)
[因为rwlock, brlock在多核性能下降.需要个高性能的锁](https://www.ibm.com/developerworks/cn/linux/l-rcu/)

## How to use RCU
list rcu

## requirements
(1) support for concurrent readers, even during updates;
(2) low computation and storage overhead; and 
(3) deterministic completion time.
## Rdvantages of RCU
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


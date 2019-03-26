---
tags: [ kernel ] 
title: The Journey to RCU
date: 2015-05-24T09:52:12+08:00 
category: cs
---

# RCU
## Causality
Read Copy Update HOWTO
## Structure
[What is RCU?](http://www.rdrop.com/users/paulmck/RCU/whatisRCU.html)
[What is RCU?  --  "Read, Copy, Update"](https://www.kernel.org/doc/Documentation/RCU/whatisRCU.txt)
[A Tour Through RCU's Requirements](https://www.kernel.org/doc/Documentation/RCU/Design/Requirements/Requirements.html)
## History
[As of March 2013: Simplifying RCU](https://lwn.net/Articles/541037/)
[RCU dissertation.2004.07](http://www2.rdrop.com/users/paulmck/RCU/RCUdissertation.2004.07.14e1.pdf)


# Classic RCU
commit c17ef45342cc033fdf7bdd5b28615e0090f8d2e7
Author: Paul E. McKenney <paulmck@linux.vnet.ibm.com>
Date:   Tue Jun 23 17:12:47 2009 -0700
    rcu: Remove Classic RCU
[Brief Overview of Classic RCU Implementation](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)
[Linux2.6.11版本：classic RCU的实现](http://www.wowotech.net/kernel_synchronization/linux2-6-11-RCU.html)

# Grace period
Documentation/RCU/rcu.txt
[URCU: any period of time during which each reader thread resides in at least one quiescent state is called a grace period.](https://lwn.net/Articles/573424/)
[Start a New Grace Period](https://lwn.net/Articles/305782/#Start%20a%20new%20grace%20period.)
[... after each CPU has passed through at least one quiescent state, the RCU grace period ends.](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)

# Quiescent state
[... after all the CPUs in the system have gone through at least one "quiescent" state (such as context switch, idle loop, or user code)](http://lse.sourceforge.net/locking/rcu/HOWTO/descrip.html)
[URCU: Any line of code not in an RCU read-side critical section is termed a quiescent state](https://lwn.net/Articles/573424/)
[The rcu and rcu_bh flavors of RCU have different sets of quiescent states.](https://lwn.net/Articles/305782/#Pass%20through%20a%20quiescent%20state.)
[Using Quiescent States to Reclaim Memory](https://preshing.com/20160726/using-quiescent-states-to-reclaim-memory/)

# granularity of waiting
[The great advantage of RCU ... without having to explicitly track each and every one of them](https://lwn.net/Articles/262464/#Wait%20For%20Pre-Existing%20RCU%20Readers%20to%20Complete) 
In RCU's case, the things waited on are called "RCU read-side critical sections". ditto.

# Preemptiable RCU
[The 1st: Read-copy-update for realtime](https://lwn.net/Articles/201195/) [Papper](http://www.rdrop.com/users/paulmck/RCU/OLSrtRCU.2006.08.11a.pdf)
[The design of preemptible read-copy-update](https://lwn.net/Articles/253651/)
The RCU implementation for the -rt patchset is unusual in that it permits read-side critical sections to be preempted and to be blocked waiting for locks. However, it does not handle general blocking (for example, via the wait_event() primitive): if you need that, you should instead use SRCU.
[Realtime RCU](http://www.rdrop.com/users/paulmck/RCU/realtimeRCU.2005.04.23a.pdf)
[RCU-preempt: What happens on a context switch](http://www.joelfernandes.org/linuxinternals/2018/05/10/5-rcu-preempt-context-switch.html)
## Implementation
+#define RCU_READ_UNLOCK_NEED_QS (1 << 2) /* RCU core needs CPU response. */
f41d911f8c49a5d65c86504c19e8204bb605c4fd
need_qs
1d082fd061884a587c490c4fc8a2056ce1e47624

# SRCU 
[Sleepable RCU](https://lwn.net/Articles/202847/)

# Tasks RCU
[The RCU-tasks subsystem](https://lwn.net/Articles/607117/)

# Tiny RCU / RCU: The Bloatwatch Edition
[RCU: The Bloatwatch Edition](https://lwn.net/Articles/323929/)
[rcu: Add a TINY_PREEMPT_RCU](https://lwn.net/Articles/396767/)
[rcu: Remove TINY_PREEMPT_RCU](https://lore.kernel.org/patchwork/patch/373048/)

# Tree RCU / Hierarchical RCU
[Hierarchical RCU](https://lwn.net/Articles/305782)
[Tree RCU Grace Period Memory Ordering Components ](https://www.kernel.org/doc/Documentation/RCU/Design/Memory-Ordering/Tree-RCU-Memory-Ordering.html)
[A Tour Through TREE_RCU's Data Structures](https://www.kernel.org/doc/Documentation/RCU/Design/Data-Structures/Data-Structures.html)
[A Tour Through TREE_RCU's Expedited Grace Periods](https://www.kernel.org/doc/Documentation/RCU/Design/Expedited-Grace-Periods/Expedited-Grace-Periods.html)
[Tree preempt RCU: 3.0 and RCU: what went wrong](https://lwn.net/Articles/453002/)

# RCU bh
[Questions on rcu-bh design](https://lwn.net/ml/linux-kernel/CAJWu+oqCun1Ae6GqPxnS+eCDi3jadGPp+MO8TjOWgs+AiAh79A@mail.gmail.com/)


# Priority-Boosting RCU
[Priority-Boosting RCU Read-Side Critical Sections](https://lwn.net/Articles/220677/)
CONFIG_RCU_BOOST

# RCU API
[RCU part 3: the RCU API, 2008 edition](https://lwn.net/Articles/264090/)
[The RCU API, 2010 Edition](https://lwn.net/Articles/418853/)
[The RCU API, 2014 Edition](https://lwn.net/Articles/609904/)
[The RCU API, 2019 edition](https://lwn.net/Articles/777036/)

# RCU stalls
[Decoding Those Inscrutable RCU CPU Stall Warnings](https://www.youtube.com/watch?v=23_GOr8Sz-E)
update_process_times->rcu_check_callbacks->rcu_pending->__rcu_pending->check_cpu_stall->print_other_cpu_stall
Documentation/RCU/stallwarn.txt

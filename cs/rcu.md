---
tags: [ kernel ] 
title: The Journey to RCU
date: 2015-05-24T09:52:12+08:00 
category: cs
---


# Network RX  path
commit 2d331915a04144dad738e725769d8fac06ef6155
Author: Eric Dumazet <edumazet@google.com>
Date:   Fri Apr 1 08:52:15 2016 -0700
    tcp/dccp: use rcu locking in inet_diag_find_one_icsk()
    RX packet processing holds rcu_read_lock(), so we can remove
    pairs of rcu_read_lock()/rcu_read_unlock() in lookup functions
    if inet_diag also holds rcu before calling them.
    This is needed anyway as __inet_lookup_listener() and
    inet6_lookup_listener() will soon no longer increment
    refcount on the found listener.
    Signed-off-by: Eric Dumazet <edumazet@google.com>
    Signed-off-by: David S. Miller <davem@davemloft.net>

5142 static int process_backlog(struct napi_struct *napi, int quota)
...
5160                 while ((skb = __skb_dequeue(&sd->process_queue))) {
5161                         rcu_read_lock();
5162                         __netif_receive_skb(skb);
5163                         rcu_read_unlock();
Refs: v4.1-12249-g2c17d27c36dc
Author:     Julian Anastasov <ja@ssi.bg>
AuthorDate: Thu Jul 9 09:59:10 2015 +0300
Commit:     David S. Miller <davem@davemloft.net>
CommitDate: Fri Jul 10 18:16:36 2015 -0700

    net: call rcu_read_lock early in process_backlog

    Incoming packet should be either in backlog queue or
    in RCU read-side section. Otherwise, the final sequence of
    flush_backlog() and synchronize_net() may miss packets
    that can run without device reference:

    CPU 1                  CPU 2
                           skb->dev: no reference
                           process_backlog:__skb_dequeue
                           process_backlog:local_irq_enable

    on_each_cpu for
    flush_backlog =>       IPI(hardirq): flush_backlog
                           - packet not found in backlog

                           CPU delayed ...
    synchronize_net
    - no ongoing RCU
    read-side sections

    netdev_run_todo,
    rcu_barrier: no
    ongoing callbacks
                           __netif_receive_skb_core:rcu_read_lock
                           - too late
    free dev
                           process packet for freed dev

    Fixes: 6e583ce5242f ("net: eliminate refcounting in backlog queue")
    Cc: Eric W. Biederman <ebiederm@xmission.com>
    Cc: Stephen Hemminger <stephen@networkplumber.org>
    Signed-off-by: Julian Anastasov <ja@ssi.bg>
    Signed-off-by: David S. Miller <davem@davemloft.net>

# History
## 2001 [Read-Copy Update on ols](https://www.kernel.org/doc/ols/2001/read-copy.pdf)
[As of March 2013: Simplifying RCU](https://lwn.net/Articles/541037/)
[RCU dissertation.2004.07](http://www2.rdrop.com/users/paulmck/RCU/RCUdissertation.2004.07.14e1.pdf)
## 2008
[RCU part 3: the RCU API, 2008 edition](https://lwn.net/Articles/264090/)
## 2010
[The RCU API, 2010 Edition](https://lwn.net/Articles/418853/)
## 2014
[The RCU API, 2014 Edition](https://lwn.net/Articles/609904/)
## 2019
[The RCU API, 2019 edition](https://lwn.net/Articles/777036/)


# Formal cause
[A Tour Through RCU's Requirements](https://www.kernel.org/doc/Documentation/RCU/Design/Requirements/Requirements.html)
[A Critical RCU Safety Property is... Ease of Use!](http://www.rdrop.com/~paulmck/RCU/rcu-exploit.2019.05.01a.pdf)
[What is RCU?](http://www.rdrop.com/users/paulmck/RCU/whatisRCU.html)
[What is RCU?  --  "Read, Copy, Update"](https://www.kernel.org/doc/Documentation/RCU/whatisRCU.txt)
[Requirements for RCU part 1: the fundamentals](https://lwn.net/Articles/652156/)
[RCU in 2019](http://www.joelfernandes.org/joel/slides/RCU_in_2019_KernelRecipes.pdf)
[Kernel Recipes 2019 - RCU in 2019](https://www.youtube.com/watch?v=bsyXDAouI6E)

## RCU and dynticks
[Status of Linux dynticks](http://ertl.jp/~shinpei/conf/ospert13/slides/FredericWeisbecker.pdf)
[Full dynticks status - Frederic Weisbecker, Red Hat](https://www.youtube.com/watch?v=G3jHP9kNjwc)

## Grace period
Documentation/RCU/rcu.txt
[URCU: any period of time during which each reader thread resides in at least one quiescent state is called a grace period.](https://lwn.net/Articles/573424/)
[Start a New Grace Period](https://lwn.net/Articles/305782/#Start%20a%20new%20grace%20period.)
[... after each CPU has passed through at least one quiescent state, the RCU grace period ends.](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)
[Must-read: Expedited Grace Period Design](https://www.kernel.org/doc/Documentation/RCU/Design/Expedited-Grace-Periods/Expedited-Grace-Periods.html)

## Quiescent state
[... after all the CPUs in the system have gone through at least one "quiescent" state (such as context switch, idle loop, or user code)](http://lse.sourceforge.net/locking/rcu/HOWTO/descrip.html)
[URCU: Any line of code not in an RCU read-side critical section is termed a quiescent state](https://lwn.net/Articles/573424/)
[The rcu and rcu_bh flavors of RCU have different sets of quiescent states.](https://lwn.net/Articles/305782/#Pass%20through%20a%20quiescent%20state.)
[Using Quiescent States to Reclaim Memory](https://preshing.com/20160726/using-quiescent-states-to-reclaim-memory/)

## granularity of waiting
[The great advantage of RCU ... without having to explicitly track each and every one of them](https://lwn.net/Articles/262464/#Wait%20For%20Pre-Existing%20RCU%20Readers%20to%20Complete) 
In RCU's case, the things waited on are called "RCU read-side critical sections". ditto.

# Classic RCU
commit c17ef45342cc033fdf7bdd5b28615e0090f8d2e7
Author: Paul E. McKenney <paulmck@linux.vnet.ibm.com>
Date:   Tue Jun 23 17:12:47 2009 -0700
    rcu: Remove Classic RCU
    Remove Classic RCU, given that the combination of Tree RCU and
    the proposed Bloatwatch RCU do everything that Classic RCU can
    with fewer bugs.
tags/v2.6.32-rc1~724^2~29
[Brief Overview of Classic RCU Implementation](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)
[Linux2.6.11版本：classic RCU的实现](http://www.wowotech.net/kernel_synchronization/linux2-6-11-RCU.html)

# RCU sched
commit 9b06e818985d139fd9e82c28297f7744e1b484e1
Refs: v2.6.12-rc3-362-g9b06e818985d
Author:     Paul E. McKenney <paulmck@us.ibm.com>
AuthorDate: Sun May 1 08:59:04 2005 -0700
Commit:     Linus Torvalds <torvalds@ppc970.osdl.org>
CommitDate: Sun May 1 08:59:04 2005 -0700
    [PATCH] Deprecate synchronize_kernel, GPL replacement
    The synchronize_kernel() primitive is used for quite a few different purposes:
    waiting for RCU readers, waiting for NMIs, waiting for interrupts, and so on.
    This makes RCU code harder to read, since synchronize_kernel() might or might
    not have matching rcu_read_lock()s.  This patch creates a new
    synchronize_rcu() that is to be used for RCU readers and a new
    synchronize_sched() that is used for the rest.  These two new primitives
    currently have the same implementation, but this is might well change with
    additional real-time support. 
+ * synchronize_sched - block until all CPUs have exited any non-preemptive
+ * kernel code sequences.
+ * This means that all preempt_disable code sequences, including NMI and
+ * hardware-interrupt handlers, in progress on entry will have completed
+ * before this primitive returns.  However, this does not guarantee that
+ * softirq handlers will have completed, since in some kernels
+ * This primitive provides the guarantees made by the (deprecated)
+ * synchronize_kernel() API.  In contrast, synchronize_rcu() only
+ * guarantees that rcu_read_lock() sections will have completed.

## RCU is mapped to either RCU-sched or RCU-preempt depending on configuration.
[[RFC,-tip,0/4] RCU cleanups and simplified preemptable RCU](https://lore.kernel.org/patchwork/cover/164913/)
o	Rename variables and functions so that RCU-sched is an
	underlying definition, along with RCU-bh and (when so
	configured) RCU-preempt.  RCU then maps to either RCU-sched
	or RCU-preempt, depending on configuration.
commit d6714c22b43fbcbead7e7b706ff270e15f04a791
Refs: v2.6.31-rc6-15-gd6714c22b43f
Author:     Paul E. McKenney <paulmck@linux.vnet.ibm.com>
AuthorDate: Sat Aug 22 13:56:46 2009 -0700
Commit:     Ingo Molnar <mingo@elte.hu>
CommitDate: Sun Aug 23 10:32:37 2009 +0200
    rcu: Renamings to increase RCU clarity
    Make RCU-sched, RCU-bh, and RCU-preempt be underlying
    implementations, with "RCU" defined in terms of one of the
    three.  Update the outdated rcu_qsctr_inc() names, as these
    functions no longer increment anything.

## Quiescent states for RCU-sched
[Verification of Tree-Based Hierarchical Read-Copy Update in the Linux Kernel](http://www.kroening.com/papers/date2018-rcu.pdf)
The non-preemptible RCU-sched flavor’s quiescent states
apply to CPUs, and are user-space execution, context switch,
idle, and offline state. Therefore, RCU-sched only needs to
track tasks and interrupt handlers that are actually running
because blocked and preempted tasks are always in quiescent states. Thus, RCU-sched needs only track CPU states.

## RCU stalls
[Decoding Those Inscrutable RCU CPU Stall Warnings](https://www.youtube.com/watch?v=23_GOr8Sz-E)
update_process_times->rcu_check_callbacks->rcu_pending->__rcu_pending->check_cpu_stall->print_other_cpu_stall
Documentation/RCU/stallwarn.txt

# RCU-preempt Preemptiable RCU
[Must-read: RCU-preempt: What happens on a context switch](http://www.joelfernandes.org/linuxinternals/2018/05/10/5-rcu-preempt-context-switch.html)
[Towards Hard Realtime Response from the Linux Kernel on SMP Hardware](http://www.rdrop.com/users/paulmck/RCU/realtimeRCU.2005.04.23a.pdf)
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

## RCU-preempt and sleeping
[A recent discussion on LKML clarified to me that “preempted to run something else” not only covers involuntary preemption but also voluntarily sleeping.](http://www.joelfernandes.org/linuxinternals/2018/05/10/5-rcu-preempt-context-switch.html)
[The exception is -rt "spinlock" acquisition.  If the "spinlock" is held,](joelfernandes.org/linuxinternals/2018/05/10/5-rcu-preempt-context-switch.html)
the task acquiring it will block, which is legal within an RCU-preempt
read-side critical section.
This exception is why I define bad things in terms of lack of
susceptibility to priority boosting instead of sleeping.
## Priority-Boosting RCU
[Priority-Boosting RCU Read-Side Critical Sections](https://lwn.net/Articles/220677/)
CONFIG_RCU_BOOST

## The motivation for that CPU reports QS
@冯博群 你好，请教一个问题，对于rcu-preempt， CPU report QS的意义是什么？ 我理解只有task QS 才不block GP。
冯博群: qs都是CPU的，每个task都要report qs的话，那记录的结构得多复杂; rcu preempt是搞了一个list用来记录block当前qs的task; 当前gp; 过gp的条件就是list为空，且所有的CPU都report过qs;你说得概念上没啥问题，但是实现中不是这样作的.

# RCU bh
[Questions on rcu-bh design](https://lwn.net/ml/linux-kernel/CAJWu+oqCun1Ae6GqPxnS+eCDi3jadGPp+MO8TjOWgs+AiAh79A@mail.gmail.com/)
## [Motivation for RCU-bh and vs RCU-preempt](https://www.mail-archive.com/linux-kernel@vger.kernel.org/msg1682346.html)
[From kernel doc rcu design](https://www.kernel.org/doc/Documentation/RCU/Design/Requirements/Requirements.html#Bottom-Half%20Flavor)
The softirq-disable (AKA “bottom-half”, hence the “_bh” abbreviations) flavor of RCU, or RCU-bh, was developed by Dipankar Sarma to provide a flavor of RCU that could withstand the network-based denial-of-service attacks researched by Robert Olsson. These attacks placed so much networking load on the system that some of the CPUs never exited softirq execution, which in turn prevented those CPUs from ever executing a context switch, which, in the RCU implementation of that time, prevented grace periods from ever ending. The result was an out-of-memory condition and a system hang.
The solution was the creation of RCU-bh, which does local_bh_disable() across its read-side critical sections, and which uses *the transition from one type of softirq processing to another as a quiescent state in addition to context switch, idle, user mode, and offline*. This means that RCU-bh grace periods can complete even when some of the CPUs execute in softirq indefinitely, thus allowing algorithms based on RCU-bh to withstand network-based denial-of-service attacks.

# SRCU 
[Sleepable RCU](https://lwn.net/Articles/202847/)

# Tasks RCU
[The RCU-tasks subsystem](https://lwn.net/Articles/607117/)

# Tiny RCU / RCU: The Bloatwatch Edition
[RCU: The Bloatwatch Edition](https://lwn.net/Articles/323929/)
[rcu: Add a TINY_PREEMPT_RCU](https://lwn.net/Articles/396767/)
[rcu: Remove TINY_PREEMPT_RCU](https://lore.kernel.org/patchwork/patch/373048/)

# Tree RCU / Hierarchical RCU
[Verification of Tree-Based Hierarchical Read-Copy Update in the Linux Kernel](http://www.kroening.com/papers/date2018-rcu.pdf)
[Hierarchical RCU](https://lwn.net/Articles/305782)
[Tree RCU Grace Period Memory Ordering Components ](https://www.kernel.org/doc/Documentation/RCU/Design/Memory-Ordering/Tree-RCU-Memory-Ordering.html)
[A Tour Through TREE_RCU's Data Structures](https://www.kernel.org/doc/Documentation/RCU/Design/Data-Structures/Data-Structures.html)
[A Tour Through TREE_RCU's Expedited Grace Periods](https://www.kernel.org/doc/Documentation/RCU/Design/Expedited-Grace-Periods/Expedited-Grace-Periods.html)
[Tree preempt RCU: 3.0 and RCU: what went wrong](https://lwn.net/Articles/453002/)

# list and rcu
[Using RCU to Protect Read-Mostly Linked Lists](https://www.kernel.org/doc/Documentation/RCU/listRCU.rst)
## hlist-nulls
commit bbaffaca4810de1a25e32ecaf836eeaacc7a3d11
Refs: v2.6.28-rc4-513-gbbaffaca4810
Author:     Eric Dumazet <dada1@cosmosbay.com>
AuthorDate: Sun Nov 16 19:37:55 2008 -0800
Commit:     David S. Miller <davem@davemloft.net>
CommitDate: Sun Nov 16 19:37:55 2008 -0800
    rcu: Introduce hlist_nulls variant of hlist
    hlist uses NULL value to finish a chain.
    hlist_nulls variant use the low order bit set to 1 to signal an end-of-list marker.
    This allows to store many different end markers, so that some RCU lockless
    algos (used in TCP/UDP stack for example) can save some memory barriers in
    fast paths.
[Usage of hilsit-nulls in kernel doc](https://www.kernel.org/doc/Documentation/RCU/rculist_nulls.txt)

# slab and rcu
tglx: commit 77631565ae40a44f23eac2e9c440cbceed8962a7
Author:     Hugh Dickins <hugh@veritas.com>
AuthorDate: Mon Aug 23 21:24:22 2004 -0700
Commit:     Linus Torvalds <torvalds@ppc970.osdl.org>
CommitDate: Mon Aug 23 21:24:22 2004 -0700
    [PATCH] rmaplock: SLAB_DESTROY_BY_RCU

commit d7de4c1dc3a2faca0bf05d9e342f885cb2696766
Refs: v2.6.28-rc4-307-gd7de4c1dc3a2
Author:     Peter Zijlstra <a.p.zijlstra@chello.nl>
AuthorDate: Thu Nov 13 20:40:12 2008 +0200
Commit:     Pekka Enberg <penberg@cs.helsinki.fi>
CommitDate: Thu Nov 13 20:49:02 2008 +0200
    slab: document SLAB_DESTROY_BY_RCU
    Explain this SLAB_DESTROY_BY_RCU thing..

## SLAB_TYPESAFE_BY_RCU
commit 5f0d5a3ae7cff0d7fa943c199c3a2e44f23e1fac
Refs: v4.11-rc2-1-g5f0d5a3ae7cf
Author:     Paul E. McKenney <paulmck@linux.vnet.ibm.com>
AuthorDate: Wed Jan 18 02:53:44 2017 -0800
Commit:     Paul E. McKenney <paulmck@linux.vnet.ibm.com>
CommitDate: Tue Apr 18 11:42:36 2017 -0700
    mm: Rename SLAB_DESTROY_BY_RCU to SLAB_TYPESAFE_BY_RCU
    A group of Linux kernel hackers reported chasing a bug that resulted
    from their assumption that SLAB_DESTROY_BY_RCU provided an existence
    guarantee, that is, that no block from such a slab would be reallocated
    during an RCU read-side critical section.  Of course, that is not the
    case.  Instead, SLAB_DESTROY_BY_RCU only prevents freeing of an entire
    slab of blocks.

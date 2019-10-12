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

# list rcu
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

# slab rcu
commit 77631565ae40a44f23eac2e9c440cbceed8962a7
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



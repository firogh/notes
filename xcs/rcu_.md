---
tags: [ kernel ] 
title: The Journey to RCU
date: 2015-05-24T09:52:12+08:00 
category: cs
---

# Reference
RCU, cond_resched(), and performance regressions https://lwn.net/Articles/603252/
[Using Quiescent States to Reclaim Memory](https://preshing.com/20160726/using-quiescent-states-to-reclaim-memory/)
[A Tour Through RCU's Requirements](https://www.kernel.org/doc/Documentation/RCU/Design/Requirements/Requirements.html)
[A Critical RCU Safety Property is... Ease of Use!](http://www.rdrop.com/~paulmck/RCU/rcu-exploit.2019.05.01a.pdf)
[What is RCU?](http://www.rdrop.com/users/paulmck/RCU/whatisRCU.html)
[What is RCU?  --  "Read, Copy, Update"](https://www.kernel.org/doc/Documentation/RCU/whatisRCU.txt)
[Requirements for RCU part 1: the fundamentals](https://lwn.net/Articles/652156/)
## History
2001 [Read-Copy Update on ols](https://www.kernel.org/doc/ols/2001/read-copy.pdf)
[As of March 2013: Simplifying RCU](https://lwn.net/Articles/541037/)
[RCU dissertation.2004.07](http://www2.rdrop.com/users/paulmck/RCU/RCUdissertation.2004.07.14e1.pdf)
[RCU part 3: the RCU API, 2008 edition](https://lwn.net/Articles/264090/)
[The RCU API, 2010 Edition](https://lwn.net/Articles/418853/)
[The RCU API, 2014 Edition](https://lwn.net/Articles/609904/)
[The RCU API, 2019 edition](https://lwn.net/Articles/777036/)
## RCU Talk by Joel Fernandes
[RCU in 2019](http://www.joelfernandes.org/joel/slides/RCU_in_2019_KernelRecipes.pdf)
[Kernel Recipes 2019 - RCU in 2019](https://www.youtube.com/watch?v=bsyXDAouI6E)
https://www.slideshare.net/ennael/kernel-recipes-2019-rcu-in-2019-joel-fernandes
## And
Verification of the Tree-Based Hierarchical Read-Copy Update in the Linux Kernel: https://arxiv.org/pdf/1610.03052.pdf
https://www.kernel.org/doc/Documentation/RCU/Design/Data-Structures/Data-Structures.html#The%20rcu_segcblist%20Structure

# The problems - refrence
The fundermental problems underlying RCU is how to make sure if there is a refrence to the data which is going to be reclaim.
So RCU is essentially used as resource-reclamation mechanism. And we know there are 3 popular ways hazard pointer, reference counting, Quiescent state to do rerouce reclamation. RCU use quiescent state.

# Quiescent state in Linux kernel
[URCU: Any line of code not in an RCU read-side critical section is termed a quiescent state](https://lwn.net/Articles/573424/)
[... after all the CPUs in the system have gone through at least one "quiescent" state (such as context switch, idle loop, or user code)](http://lse.sourceforge.net/locking/rcu/HOWTO/descrip.html)
[The rcu and rcu_bh flavors of RCU have different sets of quiescent states.](https://lwn.net/Articles/305782/#Pass%20through%20a%20quiescent%20state.)
rcu_flavor_sched_clock_irq rcu_qs
rcu_report_qs_rnp
## CPU vs QS
@冯博群 你好，请教一个问题，对于rcu-preempt， CPU report QS的意义是什么？ 我理解只有task QS 才不block GP。
冯博群: qs都是CPU的，每个task都要report qs的话，那记录的结构得多复杂; rcu preempt是搞了一个list用来记录block当前qs的task; 当前gp; 过gp的条件就是list为空，且所有的CPU都report过qs;你说得概念上没啥问题，但是实现中不是这样作的.
## EQS
rcu_eqs_exit
rcu_is_cpu_rrupt_from_idle: this idle is RCU-idle not PF_IDLE.
smp_load_acquire?
RCU implementations that avoid unnecessarily awakening dyntick-idle CPUs will mark those CPUs as being in an extended quiescent state from Hierarchical RCU
: https://lwn.net/Articles/305782/
## EQS vs interrupt vs read-side section

## heavy_qs
rcu_momentary_dyntick_idle

# Grace period - time to reclaim
[URCU: any period of time during which each reader thread resides in at least one quiescent state is called a grace period.](https://lwn.net/Articles/573424/)
[... after each CPU has passed through at least one quiescent state, the RCU grace period ends.](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)
## GP vs QS vs read vs updater
GP: rcu_state
QS: rcu_data
data: rcu_head
1. when to start GP
Updater want to free data if no GP is in-progress.
## GP vs Data(callback)
continuos Data on GP. Segmented.
## Kernel move GP handling to kthread
 "The reason for moving RCU grace-period handling to a kernel thread was to improve real-time latency
[The new visibility of RCU processing](https://lwn.net/Articles/518953/)
## gp_seq
struct rcu_data.gp_seq rcu_data:a
commit e3663b1024d1f94688e5233440ad67a9bc10b94e
Refs: v3.19-rc1-6-ge3663b1024d1
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Mon Dec 8 20:26:55 2014 -0800
    rcu: Handle gpnum/completed wrap while dyntick idle
# nocb
[Relocating RCU callbacks](https://lwn.net/Articles/522262/)
## rcuog
commit 12f54c3a8410102afb96ed437aebe7f1d87f399f
Refs: v5.3-rc2-35-g12f54c3a8410
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Fri Mar 29 16:43:51 2019 -0700
    rcu/nocb: Provide separate no-CBs grace-period kthreads

## nocb group leader
commit fbce7497ee5af800a1c350c73f3c3f103cb27a15
Refs: v3.16-rc1-3-gfbce7497ee5a
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Tue Jun 24 09:26:11 2014 -0700
    rcu: Parallelize and economize NOCB kthread wakeups
__call_rcu_nocb_enqueue replaced by __call_rcu_nocb_wake
do_nocb_deferred_wakeup

rcu_gp_kthread_wake

# non-context-switch quiescent
/*
 * This function is invoked from each scheduling-clock interrupt,
 * and checks to see if this CPU is in a non-context-switch quiescent
 * state, for example, user mode or idle loop.  It also schedules RCU
 * core processing.  If the current grace period has gone on too long,
 * it will ask the scheduler to manufacture a context switch for the sole
 * purpose of providing a providing the needed quiescent state.
 */
void rcu_sched_clock_irq(int user)


# Deffered QS
rcu_flavor_sched_clock_irq
rcu_preempt_deferred_qs
Reporting of a deferred QS reporting (when rcu_read_unlock() could not help).

# Memory ordering
A Tour Through TREE_RCU’s Grace-Period Memory Ordering https://www.kernel.org/doc/html/latest/RCU/Design/Memory-Ordering/Tree-RCU-Memory-Ordering.html

# Start a grapce period
[Start a New Grace Period](https://lwn.net/Articles/305782/#Start%20a%20new%20grace%20period.)

# Expedite grace period
[Expedited Grace Period Design](https://www.kernel.org/doc/Documentation/RCU/Design/Expedited-Grace-Periods/Expedited-Grace-Periods.html)

# Process data
## Callbacks vs GP vs segments
GP is completed, current, next
commit 5127bed588a2f8f3a1f732de2a8a190b7df5dce3
Author: Lai Jiangshan <laijs@cn.fujitsu.com>
Date:   Sun Jul 6 17:23:59 2008 +0800
    rcu classic: new algorithm for callbacks-processing(v2)

### Next segment list
Finally, the RCU NEXT TAIL segment contains callbacks that are not yet associated with any grace period. from V..
RCU_NEXT_TAIL: Callbacks that have not yet been associated with a grace period. from design/data-structure.
commit 64db4cfff99c04cd5f550357edcc8780f96b54a2
Refs: v2.6.28-rc8-92-g64db4cfff99c
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Thu Dec 18 21:55:32 2008 +0100
Commit:     Ingo Molnar <mingo@elte.hu>
CommitDate: Thu Dec 18 21:56:04 2008 +0100
    "Tree RCU": scalable classic RCU implementation
+        * [*nxttail[RCU_NEXT_READY_TAIL], NULL = *nxttail[RCU_NEXT_TAIL]):
+        *      Entries that might have arrived after current GP ended
+        * [*nxttail[RCU_WAIT_TAIL], *nxttail[RCU_NEXT_READY_TAIL]):
+        *      Entries known to have arrived before current GP ended
commit 15fecf89e46a962ccda583d919e25d9da7bf0723
Refs: v4.11-rc2-13-g15fecf89e46a
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Wed Feb 8 12:36:42 2017 -0800
Commit:     Paul E. McKenney <paulmck@kernel.org>
CommitDate: Tue Apr 18 11:38:18 2017 -0700
    srcu: Abstract multi-tail callback list handling
+ * [*tails[RCU_NEXT_READY_TAIL], *tails[RCU_NEXT_TAIL]):
+ *     Callbacks that might have arrived after the next GP started.
## Adavnce segments
rcu_advance_cbs
rcu_segcblist_advance
note_gp_changes 
If an old grace period has ended,
rcu advance cbs() is invoked to advance all callbacks,
otherwise, rcu accelerate cbs() is invoked to assign a
grace period to any recently arrived callbacks. If a new grace
period has started, ->passed quiesce is set to zero, and if
in addition RCU is waiting for a quiescent state from this
CPU, ->qs pending is set to one, so that a new quiescent
state will be detected for the new grace period.
https://github.com/lihaol/verify-treercu/blob/master/main.c
## Accelerate
rcu_segcblist_accelerate - the core function
## Batch processing
## RCU callbacks offload
[Relocating RCU callbacks](https://lwn.net/Articles/522262/)
API 14: https://lwn.net/Articles/609904/

# RCU CPU stall
https://www.kernel.org/doc/Documentation/RCU/stallwarn.txt
[Decoding Those Inscrutable RCU CPU Stall Warnings](https://www.youtube.com/watch?v=23_GOr8Sz-E)
update_process_times rcu_pending print_other_cpu_stall print_cpu_stall_info

# RCU and dynticks
[Status of Linux dynticks](http://ertl.jp/~shinpei/conf/ospert13/slides/FredericWeisbecker.pdf)
[Full dynticks status - Frederic Weisbecker, Red Hat](https://www.youtube.com/watch?v=G3jHP9kNjwc)
[RCU and dynticks-idle mode](http://www.joelfernandes.org/linuxinternals/2018/06/15/rcu-dynticks.html)
## Upcall
/*
 * Exit an RCU extended quiescent state, which can be either the
 * idle loop or adaptive-tickless usermode execution.
 *
 * We crowbar the ->dynticks_nmi_nesting field to DYNTICK_IRQ_NONIDLE to
 * allow for the possibility of usermode upcalls messing up our count of
 * interrupt nesting level during the busy period that is just now starting.
 */
static void noinstr rcu_eqs_exit(bool user)
See desgin/data-structures
However, it turns out that when running in non-idle kernel context, the Linux kernel is fully capable of entering interrupt handlers that never exit and perhaps also vice versa. Therefore, whenever the ->dynticks_nesting field is incremented up from zero, the ->dynticks_nmi_nesting field is set to a large positive number, and whenever the ->dynticks_nesting field is decremented down to zero, the the ->dynticks_nmi_nesting field is set to zero. Assuming that the number of misnested interrupts is not sufficient to overflow the counter, this approach corrects the ->dynticks_nmi_nesting field every time the corresponding CPU enters the idle loop from process context.

## Translation for Hierarchical RCU
rcu_enter_nohz -> rcu_idle_enter
RCU idle includes eqs which includes 
/*
 * Enter an RCU extended quiescent state, which can be either the
 * idle loop or adaptive-tickless usermode execution.
 *
 * We crowbar the ->dynticks_nmi_nesting field to zero to allow for
 * the possibility of usermode upcalls having messed up our count
 * of interrupt nesting level during the prior busy period.
 */
static noinstr void rcu_eqs_enter(bool user)
Firo: so extended quiescent state includes non-extended quiescent state.

rcu_dynticks_in_eqs
rcu_dynticks_curr_cpu_in_eqs
### Special/bottom bit in rcu_data:: dynticks
commit b8c17e6664c461e4aed545a943304c3b32dd309c
Refs: v4.11-rc2-1-gb8c17e6664c4
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Tue Nov 8 14:25:21 2016 -0800
Commit:     Paul E. McKenney <paulmck@kernel.org>
CommitDate: Tue Apr 18 11:19:22 2017 -0700
    rcu: Maintain special bits at bottom of ->dynticks counter
### rcu_data::dynticks counter? validate by warn on?
Integrating and Validating dynticks and Preemptable RCU?? https://lwn.net/Articles/279077/
输了又怎样？

# Model
A GP completed seq A
CBs to process
New GP in-progress
CBs waiting for GP seq B

# Tree RCU / Hierarchical RCU
[Hierarchical RCU](https://lwn.net/Articles/305782)
[A Tour Through TREE_RCU's Data Structures](https://www.kernel.org/doc/Documentation/RCU/Design/Data-Structures/Data-Structures.html)
[A Tour Through TREE_RCU's Expedited Grace Periods](https://www.kernel.org/doc/Documentation/RCU/Design/Expedited-Grace-Periods/Expedited-Grace-Periods.html)
[Tree preempt RCU: 3.0 and RCU: what went wrong](https://lwn.net/Articles/453002/)
[Verification of Tree-Based Hierarchical Read-Copy Update in the Linux Kernel](http://www.kroening.com/papers/date2018-rcu.pdf)

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

## Quiescent states for RCU-sched
[Verification of Tree-Based Hierarchical Read-Copy Update in the Linux Kernel](http://www.kroening.com/papers/date2018-rcu.pdf)
The non-preemptible RCU-sched flavor’s quiescent states apply to CPUs, and are user-space execution, context switch, idle, and offline state. Therefore, RCU-sched only needs to track tasks and interrupt handlers that are actually running because blocked and preempted tasks are always in quiescent states. Thus, RCU-sched needs only track CPU states.

# RCU-preempt Preemptiable RCU
[ RCU-preempt: What happens on a context switch](http://www.joelfernandes.org/linuxinternals/2018/05/10/5-rcu-preempt-context-switch.html)
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

# RCU bh
[rcu-bh design by Joel Fernandes <joelaf@google.com>](https://lore.kernel.org/lkml/CAJWu+oqCun1Ae6GqPxnS+eCDi3jadGPp+MO8TjOWgs+AiAh79A@mail.gmail.com/)
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

# Classic RCU - let it die peacefully
tags/v2.6.32-rc1~724^2~29
commit c17ef45342cc033fdf7bdd5b28615e0090f8d2e7
Author: Paul E. McKenney <paulmck@linux.vnet.ibm.com>
Date:   Tue Jun 23 17:12:47 2009 -0700
    rcu: Remove Classic RCU
    Remove Classic RCU, given that the combination of Tree RCU and
    the proposed Bloatwatch RCU do everything that Classic RCU can
    with fewer bugs.
[Brief Overview of Classic RCU Implementation](https://lwn.net/Articles/305782/#Brief%20Overview%20of%20Classic%20RCU%20Implementation)
[Linux2.6.11版本：classic RCU的实现](http://www.wowotech.net/kernel_synchronization/linux2-6-11-RCU.html)

# Jargons
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
    functions no loner increment anything.

# RCU and preempt
commit 95f0c1de3e6ed4383cc4b5f52ce4ecfb21026b49
Refs: v3.5-rc5-17-g95f0c1de3e6e
Author:     Paul E. McKenney <paulmck@kernel.org>
AuthorDate: Tue Jun 19 11:58:27 2012 -0700
Commit:     Paul E. McKenney <paulmck@kernel.org>
CommitDate: Mon Jul 2 12:34:25 2012 -0700
    rcu: Disable preemption in rcu_blocking_is_gp()
    It is time to optimize CONFIG_TREE_PREEMPT_RCU's synchronize_rcu()
    for uniprocessor optimization, which means that rcu_blocking_is_gp()
    can no longer rely on RCU read-side critical sections having disabled
    preemption.  This commit therefore disables preemption across
    rcu_blocking_is_gp()'s scan of the cpu_online_mask.
    (Updated from previous version to fix embarrassing bug spotted by
    Wu Fengguang.)


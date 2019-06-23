---
tags: [ cs ] 
title: Scheduling in operating system 
date: 2017-03-29T10:49:04+08:00 
category: cs
---
# Reference
Process scheduling in Linux -- Volker Seeker from University of Edinburgh
[A complete guide to Linux process scheduling](https://tampub.uta.fi/bitstream/handle/10024/96864/GRADU-1428493916.pdf)
https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt
[JINKYU KOO's Linux kernel scheduler](https://helix979.github.io/jkoo/post/os-scheduler/)

# General runqueues
static DEFINE_PER_CPU_SHARED_ALIGNED(struct rq, runqueues);
activate_task - move a task to the runqueue.
wake_up_new_task
ttwu_do_activate

# CFS runqueues
cfa_rq
on_list
sched_entity->on_rq, check enqueue_entity
## Leaf CFS runqueues
/*
 * leaf cfs_rqs are those that hold tasks (lowest schedulable entity in
 * a hierarchy). Non-leaf lrqs hold other higher schedulable entities
 * (like users, containers etc.)
 *
 * leaf_cfs_rq_list ties together list of leaf cfs_rq's in a cpu. This
 * list is used during load balance.
 */
int on_list;
struct list_head leaf_cfs_rq_list;
Head of list: rq->leaf_cfs_rq_list

## CFS runqueue and sched entity
set_task_rq

## on_rq
on_rq should be same as task->on_rq. It doesn't mean sched_entity is on cfs_rq, but rq.
commit fd2f4419b4cbe8fe90796df9617c355762afd6a4
Author: Peter Zijlstra <a.p.zijlstra@chello.nl>
Date:   Tue Apr 5 17:23:44 2011 +0200
    sched: Provide p->on_rq
p->on_rq on any rq.
se->on_rq on specific rq.

## CFS runqueue and task group
sched_create_group -> alloc_fair_sched_group -> init_tg_cfs_entry

# CFS core codes
git log 20b8a59f2461e

# Group scheduling
[GROUP SCHEDULER EXTENSIONS TO CFS](https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt)
[CFS调度器（3）-组调度](http://www.wowotech.net/process_management/449.html)
[Linux进程组调度机制分析](http://oenhan.com/task-group-sched)
## Two trees
task_group->parent; task_group->css.cgroup
cgroup->parent and cgroup_tg: container_of(cgroup_subsys_state(cgrp, cpu_cgroup_subsys_id), struct task_group, css);
## Task group and cgroup is 1:1
## System bootup
struct task_group root_task_group; and cpu_cgroup_create;
## Creating task_group
sched_create_group
task_group 1 : cpu 'group sched_entity'
group sched_entity 1 : 1 greoup cfs_rq
gse_CPUx's load = grq_CPUx's all se's load * task_group->shares / grq_CPU*'s all se's load
        /* rq on which this entity is (to be) queued: */
        struct cfs_rq           *cfs_rq;
        /* rq "owned" by this entity/group: */
        struct cfs_rq           *my_q;

[CFS group scheduling](https://lwn.net/Articles/240474/)
commit 29f59db3a74b0bdf78a1f5b53ef773caa82692dc
Author: Srivatsa Vaddagiri <vatsa@linux.vnet.ibm.com>
Date:   Mon Oct 15 17:00:07 2007 +0200
    sched: group-scheduler core

## Why double for_each_sched_entity
commit 2069dd75c7d0f49355939e5586daf5a9ab216db7
Author: Peter Zijlstra <a.p.zijlstra@chello.nl>
Date:   Mon Nov 15 15:47:00 2010 -0800
    sched: Rewrite tg_shares_up)

371fd7e7a56a5 (Peter Zijlstra       2010-03-24 16:38:48 +0100 1129) enqueue_task_fair(struct rq *rq, struct task_struct *p, int flags)
bf0f6f24a1ece (Ingo Molnar          2007-07-09 18:51:58 +0200 1134)     for_each_sched_entity(se) {
62fb185130e4d (Peter Zijlstra       2008-02-25 17:34:02 +0100 1135)             if (se->on_rq)
bf0f6f24a1ece (Ingo Molnar          2007-07-09 18:51:58 +0200 1136)                     break;
bf0f6f24a1ece (Ingo Molnar          2007-07-09 18:51:58 +0200 1137)             cfs_rq = cfs_rq_of(se);
88ec22d3edb72 (Peter Zijlstra       2009-12-16 18:04:41 +0100 1138)             enqueue_entity(cfs_rq, se, flags);
88ec22d3edb72 (Peter Zijlstra       2009-12-16 18:04:41 +0100 1139)             flags = ENQUEUE_WAKEUP;
bf0f6f24a1ece (Ingo Molnar          2007-07-09 18:51:58 +0200 1140)     }
8f4d37ec073c1 (Peter Zijlstra       2008-01-25 21:08:29 +0100 1141) 
2069dd75c7d0f (Peter Zijlstra       2010-11-15 15:47:00 -0800 1142)     for_each_sched_entity(se) {
2069dd75c7d0f (Peter Zijlstra       2010-11-15 15:47:00 -0800 1143)             struct cfs_rq *cfs_rq = cfs_rq_of(se);
2069dd75c7d0f (Peter Zijlstra       2010-11-15 15:47:00 -0800 1144) 
2069dd75c7d0f (Peter Zijlstra       2010-11-15 15:47:00 -0800 1145)             update_cfs_load(cfs_rq);
2069dd75c7d0f (Peter Zijlstra       2010-11-15 15:47:00 -0800 1146)             update_cfs_shares(cfs_rq);

# Wake up
[sched: lockless wake-queues](https://lkml.org/lkml/2015/4/19/111)
[Futex Scaling for Multi-core Systems](https://www.youtube.com/watch?v=-8c47dHuGIY)[Slides](https://www.slideshare.net/davidlohr/futex-scaling-for-multicore-systems)

# Program-Order guarantees
commit 8643cda549ca49a403160892db68504569ac9052
Author: Peter Zijlstra <peterz@infradead.org>
Date:   Tue Nov 17 19:01:11 2015 +0100
    sched/core, locking: Document Program-Order guarantees
## LKML discussions
[scheduler ordering bits](https://lkml.org/lkml/2015/11/2/311)
[scheduler ordering bits -v2](https://lkml.org/lkml/2015/12/3/323)
## pi_lock
commit b29739f902ee76a05493fb7d2303490fc75364f4
Author: Ingo Molnar <mingo@elte.hu>
Date:   Tue Jun 27 02:54:51 2006 -0700
    [PATCH] pi-futex: scheduler support for pi
    Add framework to boost/unboost the priority of RT tasks.

# ETC
## the wake-up lost problem
[Kernel Korner - Sleeping in the Kernel](https://www.linuxjournal.com/article/8144)
## Reordering issue
CPU0:Process A
while(!done) {
        schedule();
        set_current_state = *interruptable;
}
CPU1: Process B
done = true;
wake_up_process(A);
Ordering issue: reorder load of done and store of state;
A: load of done
B: done = true
B: wake up
A: set state
A: schedule
Check bcbd94ff481ec1d7b5c824d90df82d0faafabd35
dm crypt: fix a possible hang due to race condition on exit
## PELT
[Per-entity load tracking](https://lwn.net/Articles/531853/)

# Running time
proc_sched_show_task


# Problems
## Why scheduling?
Customers demand multitasking/concurrent
Processes are blocked

## Fairness
Unit: /proc/sys/kernel/sched_min_granularity_ns

# Conceptions
[Cpu capacity](https://android.googlesource.com/kernel/msm/+/android-msm-bullhead-3.10-marshmallow-dr/Documentation/scheduler/sched-hmp.txt)

# Running Compensator records the running process
scheduler_tick  
{
	update_rq_clock
	task_tick_fair -> entity_tick
	{
		update_curr
		{
			sum_exec_runtime - total runtime
			cfs_rq->exec_clock - cfs_rq runtime
			vruntime	- inverse proportion to the weight or priority
			update_min_vruntime
			{
				cfs_rq->curr, leftmost, min_vruntime, who is min?
			}
			cpuacct	- cpu sys/user time
		}
	}
}
# Next -> pick_next_task_fair
put_prev_entity: update_curr; insert into rb-tree; 
pick_next_entity: left most of rb-tree.
set_next_entity: remove next from tree since it will disturb inserting and deleting when it is being updated.
# Unrunnable
dequeue_task
# Resuming
try_to_wake_up->ttwu_queue->ttwu_do_activate-> or local wakeup: schedule->try_to_wake_up_local->
{
	ttwu_activate				#=== speical compensation and enqueue rq
	{
		activate_task
		p->on_rq = TASK_ON_RQ_QUEUED	#=== 1) rq for task; 2) 
	}
	ttwu_do_wakeup				#=== normal compensation
	{
		check_preempt_curr
		p->state = TASK_RUNNING;
	}
}

enqueue_task-> place_entity compensation for wakeup process
## wake up a sleep task
	se->on_rq & TASK_ON_RQ_QUEUED; deactivate_task set on_rq to 0;
	enqueue_task_fair handles group stuff
	enqueue_entity deals with sched_entity - uptodate the vruntime, load average, account load numa perfering,
	sysctl_sched_latency: the cfs pledge to the pre-existing tasks that they have 6ms to run before new task to run.
	try_to_wake_up_local for local task
	try_to_wake_up for any task
# New task
speical debit compensation: sched_fork->task_fork_fair->place_entity - compensation for new process
normal compensation: wake_up_new_task
{
	activate_task				#=== speical compensation
	check_preempt_curr			#=== normal compensation
}
# Priority
* weight
* priority
DEFAULT_PRIO
fs/proc/array.c

# Latency
1. sched_nr_latency= /proc/sys/kernel/sched_latency_ns / /proc/sys/kernel/sched_min_granularity_ns
2. if running process > sched_nr_latency, latency cannot be ensured. just focus on min granularity
## LQO
* is the difference of leftmost and rightmost smaller than sched_min_granularity_ns??
* sched_slice
# Energy
blocked & schedule
check preempt & schedule
check_preempt_tick				# new preempts curr
{
	curr running time > sched_slice		# enough time to yield. 
	curr - leftmost > sched_slice		# nice to others.
}
check_preempt_wakeup				# the wakeuped preempts curr
{
	curr - wakeuped	> sysctl_sched_wakeup_granularity;	# pass the wakeup-preempt-delay
}
# io wait
https://lwn.net/Articles/342378/
# Load avg
update_load_avg
https://en.wikipedia.org/wiki/Load_(computing)
Check External links
calc_load_fold_active
Etymology of avenrun: [average nr. of running processes during](https://elixir.bootlin.com/linux/v4.1/source/arch/s390/appldata/appldata_os.c)
# LQO
* h_nr_running and throttled
sched: Implement hierarchical task accounting for SCHED_OTHER - 953bfcd10e6f3697233e8e5128c611d275da39c1
https://groups.google.com/forum/#!topic/linux.kernel/gRzxHclMy50
 'root' 
   \ 
   'A' 
   / \ 
  t1 t2 
root.nr_running := 2
root.h_nr_running := 2
Check enqueue_task_fair()
* idle 
https://www.kernel.org/doc/Documentation/scheduler/sched-arch.txt
[improve SMP reschedule and idle routines](https://lwn.net/Articles/136065/)
TIF_POLLING_NRFLAG -> Need-Resched-Flag?
* process migration
e761b7725234276a802322549cee5255305a0930
Introduce cpu_active_map and redo sched domain managment
	When to migration
		sched_setaffinity __set_cpus_allowed_ptr manuly
		Selecting a new CPU during wak up a sleeper 
		For balancing, selecting CPU during  wake up new process in _do_fork
		execve's sched_exec 
* shceduler clock
rq->clock is nano seconds?
* clock_task and wraps
fe44d62122829959e960bc699318d58966922a69 

* START_DEBIT
no standalone commit
bf0f6f24a1ece8988b243aefe84ee613099a9245
* why ahead?
  8 /*
  9  * Place new tasks ahead so that they do not starve already running
 10  * tasks
 11  */
 12 SCHED_FEAT(START_DEBIT, true)
the tree is named timeline
* [Improving scheduler latency ](https://lwn.net/Articles/404993/)
* skip next last buddy


# Git log
e9c8431185d6c406887190519f6dbdd112641686
TASK_WAKING; see migrate_task_rq_fair and try_to_wake_up
88ec22d3edb72b261f8628226cd543589a6d5e1b
In order to remove the cfs_rq dependency from set_task_cpu() we need to ensure the task is cfs_rq invariant for all callsites.
2f950354e6d535b892f133d20bd6a8b09430424c
sched/fair: Fix fairness issue on migration
[Migrated CFS task getting an unfair advantage](http://linux.kernel.narkive.com/p15Wmn0i/migrated-cfs-task-getting-an-unfair-advantage)
30cfdcfc5f180fc21a3dad6ae3b7b2a9ee112186
curr was not kept in rb-tree

# Load balancing
[Scheduling domains](https://lwn.net/Articles/80911/)
set sd
kernel_init_freeable->
sched_init_smp->
init_sched_domains->build_sched_domains:->
__visit_domain_allocation_hell()->__sdt_alloc() alloc the sdd->sg which is used by build groups
and sg = kzalloc_node(sizeof(struct sched_group) + cpumask_size(); it covered the size of cpumask
/* Build the groups for the domains */
detach_destroy_domains
cpu_attach_domain

CONFIG_SCHED_MC=y
static noinline struct sched_domain *                                   \
sd_init_##type(struct sched_domain_topology_level *tl, int cpu)         \
{                                                                       \
        struct sched_domain *sd = *per_cpu_ptr(tl->data.sd, cpu);       \
        *sd = SD_##type##_INIT;                                         \
        SD_INIT_NAME(sd, type);                                         \
        sd->private = &tl->data;                                        \
        return sd;                                                      \
}
tl->mask(cpu)
static struct sched_domain_topology_level default_topology[] = {
#ifdef CONFIG_SCHED_SMT
        { sd_init_SIBLING, cpu_smt_mask, },
#endif
#ifdef CONFIG_SCHED_MC
        { sd_init_MC, cpu_coregroup_mask, },
#endif
#ifdef CONFIG_SCHED_BOOK
        { sd_init_BOOK, cpu_book_mask, },
#endif
        { sd_init_CPU, cpu_cpu_mask, },
        { NULL, },
};

# Throttling entities 
commit 85dac906bec3bb41bfaa7ccaa65c4706de5cfdf8
Author: Paul Turner <pjt@google.com>
Date:   Thu Jul 21 09:43:33 2011 -0700
    sched: Add support for throttling group entities
    Now that consumption is tracked (via update_curr()) we add support to throttle
    group entities (and their corresponding cfs_rqs) in the case where this is no
    run-time remaining.

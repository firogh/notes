---
tags: [ cs ] 
title: Scheduling in operating system 
date: 2017-03-29T10:49:04+08:00 
category: cs
---

# scheduling
[Scheduling (computing)](https://en.wikipedia.org/wiki/Scheduling_(computing))

# Context switch
[Evolution of the x86 context switch in Linux](https://www.maizure.org/projects/evolution_x86_context_switch_linux/index.html)
[Al Viro's new execve/kernel_thread design](https://lwn.net/Articles/520227/)
commit 0100301bfdf56a2a370c7157b5ab0fbf9313e1cd
Author: Brian Gerst <brgerst@gmail.com>
Date:   Sat Aug 13 12:38:19 2016 -0400
    sched/x86: Rewrite the switch_to() code
[Why does switch_to use push+jmp+ret to change EIP, instead of jmp directly?](https://stackoverflow.com/questions/15019986/why-does-switch-to-use-pushjmpret-to-change-eip-instead-of-jmp-directly/15024312)

# Reference
Process scheduling in Linux -- Volker Seeker from University of Edinburgh
[A complete guide to Linux process scheduling](https://tampub.uta.fi/bitstream/handle/10024/96864/GRADU-1428493916.pdf)
https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt
[JINKYU KOO's Linux kernel scheduler](https://helix979.github.io/jkoo/post/os-scheduler/)

[TIF_NEED_RESCHED: why is it needed](http://www.joelfernandes.org/linuxinternals/2016/03/20/tif-need-resched-why-is-it-needed.html)

# Latency
[Improving scheduler latency](https://lwn.net/Articles/404993/)

# General runqueues
static DEFINE_PER_CPU_SHARED_ALIGNED(struct rq, runqueues);
activate_task - move a task to the runqueue.
wake_up_new_task ttwu_do_activate
## on_rq
commit fd2f4419b4cbe8fe90796df9617c355762afd6a4
Author: Peter Zijlstra <a.p.zijlstra@chello.nl>
Date:   Tue Apr 5 17:23:44 2011 +0200
    sched: Provide p->on_rq

# CFS core codes
git log 20b8a59f2461e
sched_create_group -> alloc_fair_sched_group -> init_tg_cfs_entry

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

# rq->lock in schedule() and context_switch()
commit 3a5f5e488ceee9e08df3dff3f01b12fafc9e7e68
Author: Ingo Molnar <mingo@elte.hu>
Date:   Fri Jul 14 00:24:27 2006 -0700
    [PATCH] lockdep: core, fix rq-lock handling on __ARCH_WANT_UNLOCKED_CTXSW
+        * Since the runqueue lock will be released by the next
+        * task

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

# Load balancing
https://www.kernel.org/doc/html/latest/_sources/scheduler/sched-domains.rst.txt
[Scheduling domains](https://lwn.net/Articles/80911/)
sched_init_smp->
sched_init_domains or init_sched_domains build_sched_domains
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
sched_domain_topology_level default_topology

# Throttling entities 
commit 85dac906bec3bb41bfaa7ccaa65c4706de5cfdf8
Author: Paul Turner <pjt@google.com>
Date:   Thu Jul 21 09:43:33 2011 -0700
    sched: Add support for throttling group entities
    Now that consumption is tracked (via update_curr()) we add support to throttle
    group entities (and their corresponding cfs_rqs) in the case where this is no
    run-time remaining.

# Load tracking - PELT
[Per-entity load tracking](https://lwn.net/Articles/531853/)
commit 5b51f2f80b3b906ce59bd4dce6eca3c7f34cb1b9
Author: Paul Turner <pjt@google.com>
Date:   Thu Oct 4 13:18:32 2012 +0200
    sched: Make __update_entity_runnable_avg() fast
commit a481db34b9beb7a9647c23f2320dd38a2b1d681f
Refs: v4.11-rc2-229-ga481db34b9be
Author:     Yuyang Du <yuyang.du@intel.com>
AuthorDate: Mon Feb 13 05:44:23 2017 +0800
    sched/fair: Optimize ___update_sched_avg()

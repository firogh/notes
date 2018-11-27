---
tags: [ cs ] 
title: Scheduling in operating system 
date: 2017-03-29T10:49:04+08:00 
category: cs
---
# ref
Process scheduling in Linux -- Volker Seeker from University of Edinburgh
[A complete guide to Linux process scheduling](https://tampub.uta.fi/bitstream/handle/10024/96864/GRADU-1428493916.pdf)
https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt

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
## Etymology - avenrun
average nr. of running processes during 
https://casper.berkeley.edu/svn/trunk/roach/sw/linux/arch/s390/appldata/appldata_os.c
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
* group schedule
https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt
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

# SD
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
CONFIG_CPUSETS=y
get sd
idle_balance
for_each_domain

panic
find_busiest_group
find_next_bit
__next_cpu



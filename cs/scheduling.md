---
tags: [ cs ] 
title: Scheduling in operating system 
date: 2017-03-29T10:49:04+08:00 
category: cs
---

Customer demand -> multitasking/concurrent
Some other issues arise from multitasking/concurrent.
Scheduling is used to implement the multitasking/concurrent and solve the arose issues.
Questions we need to solve:
1. how to switch to the context of the next process?
	context_switch
2. how to determine the next process? Fairness?
2.1 There should be many shcheduling algorithms; how to manage them?
	sched_class
2.2 where to keep/find the processes?
	All the ruunale process are put in cpu_rq, but they are in different places rt_rq, dl_rq, cfs_rq, stop, idle.
	Why different palces? Simple.
2.3 If we use CFS algoritm, how does it work?
	The core source code of CFS is the members of fair_sched_class
	The main idea of CFS is compensating the process which is deficient of running CPU time.
	CFS use vruntime to reflect the CPU time a process used.
	But what makes CFS complex? Two points:
	a. CFS is compatiable with the traditional priority(the nice value).
	b. make the vruntime suitable. For example, what's the vrumtime of a new process?
3. when to perform shcheduling?
	new task, task dies
	exception, interrupt
	preempt_schedule_irq
	retint_kernel; kernel preempt
3.1 Scheduling timer
alloc_intr_gate(LOCAL_TIMER_VECTOR, apic_timer_interrupt);
apic_timer_interrupt smp_apic_timer_interrupt local_apic_timer_interrupt
tick_handle_periodic -> tick_periodic-> scheduler_tick -> tick_periodic ->.task_tick= task_tick_fair,
4. States and actions
4.1 wake up a sleep task 
	not runnable -> activate_task -> runnable ->deactivate_task-> not runnable
	se->on_rq & TASK_ON_RQ_QUEUED; deactivate_task set on_rq to 0;
	enqueue_task_fair handles group stuff
	enqueue_entity deals with sched_entity - uptodate the vruntime, load average, account load numa perfering,
	sysctl_sched_latency: the cfs pledge to the pre-existing tasks that they have 6ms to run before new task to run.
	try_to_wake_up_local for local task
	try_to_wake_up for any task
4.2 Choses a entity
	set_next_entity is, actually, setup the next entity being running. remove entity from rb-tree.
4.3 What about a new task
4.4 process migration
e761b7725234276a802322549cee5255305a0930
Introduce cpu_active_map and redo sched domain managment
	When to migration
		sched_setaffinity __set_cpus_allowed_ptr manuly
		Selecting a new CPU during wak up a sleeper 
		For balancing, selecting CPU during  wake up new process in _do_fork
		execve's sched_exec 


4.x worker
	sleep -> waking -> woken
	
4. how to deal with load balancing of multiple CPU?
5. how to control the cost of scheduling.

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

##Preemption
* Linux kernel user mode is always User preemption.
When returning to user-space from a system call.
When returning to user-space from an interrupt hander.
* Linux kernel kernel mode is coppertive when CONFIG_PREEMPT is not set.
If a task in the kernel explicitly calls schedule()
If a task in the kernel blocked (which results in a all to schedule())
* Linux kernel kernel mode is coppertive + preemptive when CONFIG_PREEMPT is set.
schedule + blcoked
When an interrupt handler exits, before returning to kernel-space.
When kernel code becomes preemptible again.


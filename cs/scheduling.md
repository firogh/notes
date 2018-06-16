---
tags: [ cs ] 
title: Scheduling in operating system 
date: 2017-03-29T10:49:04+08:00 
category: cs
---


# SD
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
# Context switch

inactive_task_frame __switch_to_asm

sp0 load_sp0 userspace stack pointer? sdm v3a 7.2.1 Task-State Segment (TSS)
No, it isn't. sp0 is kernel stack spointer. 
Comparing to __switch_to_asm()'s movq TASK_threadsp(%rsi), %rsp and popq, to examine copy_thread_tls
context_switch - switch to the new MM and the new thread's register state.
and ret_from_fork.

What about start_thread_common()?
next's ret address of switch_to() is the new ip after changing stack. 
switch_to() is the core.

See the following; it exposures lots of information sp vs sp0.

        p->thread.sp0 = (unsigned long)task_stack_page(p) + THREAD_SIZE;
        childregs = task_pt_regs(p);
        fork_frame = container_of(childregs, struct fork_frame, regs);
        frame = &fork_frame->frame;
        frame->bp = 0;
        frame->ret_addr = (unsigned long) ret_from_fork;
        p->thread.sp = (unsigned long) fork_frame;
        p->thread.io_bitmap_ptr = NULL;

# etc
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


# FAQ
[为什么Linux内核不允许在中断中休眠](http://wangcong.org/2012/06/01/-e4-b8-ba-e4-bb-80-e4-b9-88linux-e5-86-85-e6-a0-b8-e4-b8-8d-e5-85-81-e8-ae-b8-e5-9c-a8-e4-b8-ad-e6-96-ad-e4-b8-ad-e4-bc-91-e7-9c-a0-ef-bc-9f/)

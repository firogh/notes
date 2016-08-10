---
tags: [ cs ] 
title: The computational process
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# The computational process
order -> synchronization: order temporal spatial
security -> privilege level: spatial, user space process and kernel process, trap, interrupt.
real time -> execution time:long-live
performance(real time) -> priority: order time, interrupt, trap, high priority process
efficiency -> schedule out io-waiting
fairness

schedule and interrupt is the same thing.

# kernel preempt
arch/x86/kernel/entry_64.S
preempt_schedule_irq
retint_kernel

entity_key

# priority and nice
In according to  do_task_stat, we know priority = task_prio(task); nice = task_nice(task);
p->prio - MAX_RT_PRIO; = -2,  MAX_RT_PRIO = 100;so p->prio = 98;
PRIO_TO_NICE(prio)
(p)->static_prio - MAX_RT_PRIO - 20 = 0; So p->static_prio = 120;
NICE_TO_PRIO(nice)      (MAX_RT_PRIO + (nice) + 20)
Convert user-nice values [ -20 ... 0 ... 19 ] to static priority [ MAX_RT_PRIO..MAX_PRIO-1 ] 100..139, and back.
MAX_PRIO                (MAX_RT_PRIO + 40) // 140
DEFAULT_PRIO            (MAX_RT_PRIO + 20) // 120
if (task_has_rt_policy(p)) {
 p->se.load.weight = prio_to_weight[0] * 2;  /*   0 */      1024,
p->se.load.inv_weight = prio_to_wmult[0] >> 1; /*   0 */   4194304,



#define LOCAL_TIMER_VECTOR      0xef
/arch/x86/kernel/irqinit_64.c:215:	alloc_intr_gate(LOCAL_TIMER_VECTOR, apic_timer_interrupt);
apic_timer_interrupt
smp_apic_timer_interrupt
local_apic_timer_interrupt
tick_handle_periodic
tick_periodic->
scheduler_tick ->
.task_tick              = task_tick_fair,
tick_periodic ->.task_tick              = task_tick_fair,


# Protection ring
## Kernel mode:
## User mode
限制访问地址空间, 内核数据结构, I/O 指令, 中断代码等.

## Mode switch
esp既表示进程的用户栈，也表进程的内核栈，要看进程处于用户态还是内核态。
读TSS段中进程的内核态指针付给ss和esp。
把用户态SS和esp指针保存在进程的内核栈上，
从内核态返回时，弹出进程内核栈上用户态ss和esp值。
那么TSS段中进程的内核态指针是从哪里来的呢？

cpu -> tr -> tss -> esp0 => 内核栈->保存用户态cs eip esp等.
Hardware context switch.
pt_regs

## kernel stack
从system_call中, 我们可以发现:
GET_THREAD_INFO(%ebp)

master idle进程的kernel stack在init/init_task.c:init_thread_union
其他进程的kernel stack是fork产生.
this_cpu_write(kernel_stack,(unsigned long)task_stack_page(next_p) +THREAD_SIZE);
this_cpu_write(cpu_current_top_of_stack,(unsigned long)task_stack_page(next_p) +THREAD_SIZE);

# Schedule cfs
sum_runtime = sysctl_sched_min_granularity * nr_running（if 进程数 > 5）
sum_runtime = sysctl_sched_latency = 20 ms （if 进程数 <= 5） 
sysctl_sched_min_granularity = 4ms
ideal_time = sum_runtime * se.weight/cfs_rq.weight
vruntime +=  delta * NICE_0_LOAD/ se.weight;(if curr.nice!=NICE_0_LOAD)

# Idle
主处理器上的idle由原始进程(pid=0)演变而来。从处理器上的idle由init进程fork得到，但是它们的pid都为0
init_idle.

# Porcess context
## Userspace context
.txt, .data, .bss, userspace stack, heap, library.
## Kernespace context
task_struct, mm_struct, pgd, kernelspace stack.
## Hardware context
Registers

## Context switch
A context switch (also sometimes referred to as a process switch or a task switch) 
is the switching of the CPU (central processing unit) from one process or thread to another.

## 进程切换process switch
也叫上下文切换context switch只发生在内核态，必须理解而不是记住。
一个进程会用到许多cpu的寄存器比如esp，eip等等，他们被称作Hardware context硬件上下文，
如果进程是培养皿中的一条幼虫，那么硬件上下问就是培养液了。不同的虫子需要不同的培养液，
新的培养液进来，旧的培养液也需要保存，以备稍后使用。
当进程切换时，我们要保存当前被切换出去进程的硬件上下文，以备将来切换回来使用。
应该明白为什么进程切换也叫上下文切换了。怎么保存上下文呢？
在进程的task_struct结构的成员变量thread_struct结构保存被切换出去进程的硬件上下文。
我们所关心的换出的进程的esp指针也保存在thread_struct当中了。
当然，对于被切换进来的进程我们用这个进程的thread_struct成员中的esp 值更新硬件的esp寄存器，
也需要更新GDT中的TSS里的esp0,以备将来用来从用户态切换到内核态时，找到进程内核栈的栈顶指针。

## switch_to
__schedule()->swtich_to
### load TSS
进程切换只发生在内核态
/*将next进程的内核栈指针(next->thread->sp0)值更新到当前CPU的TSS中*/
load_sp0(tss, next);
task_struct.thread.sp0
thread_struct

## type of Context switch
* context switch by hardware
Old linux kernel
* context switch by software
kernel 2.6

# Common concepts
Multiprocessing for SMP

# Process flag
PF_MEMALLOC [Kill PF_MEMALLOC abuse](http://thread.gmane.org/gmane.linux.kernel/914878)

# Process user ID
## Effective user/group ID
euid:4777
egid:2777
chmod u+s cynthia
root进程执行cynthia后euid变为firo, saved user id变为root

## Example
u+s 就可以
firo@firo tmp$ iptables -L
iptables v1.4.21: can't initialize iptables table `filter': Permission denied (you must be root)
Perhaps iptables or your kernel needs to be upgraded.
chmod +s /usr/sbin/iptables
chmod -s /usr/sbin/iptables


# Scheduling strategies
multiprogramming
time-sharing(multi tasking)
real-time

# Multitasking 
## Reference
codes: comments of __schedule()
Book: Linux kernel development 

##Common concepts
* The process scheduler decides which process runs, when, and for how long.
* Multitasking: cooperative and preemptive.
* Preemption:Involuntarily suspending a running process.
* Timeslice: a slice of cpu's time.
* Yielding: The act of a process voluntarily suspending itself.
* Cooperative multitasking: a process does not stop running until it voluntary decides to do so.
* Time sharing: is cooperative mutitasking, 
* the codes that yeild the cpu when timeslice is zero while ret from syscall is also belong to user process!
so if !CONFIG_PREEMPT* is cooperative!
* CFS: Pick the task with the smallest vruntime. That's it!

##TIF_NEED_RESCHED
* The situation of set task need resched
scheduler_tick()
try_to_wake_up() when a process that has a higher priority than *current* is awakened.
Other?

## SAVE_ALL
no include cs ss ip sp eflag why?

#Deamonize
## fork, oraphan
similar &
not a goup leader inherit with parent pgid for setsid
## setsid
* session leader
* process group leader
A signal directed to a process group is delivered individually to 
all of the processes that are members of the group. 
* deatch controlling terminals(confusing)
## chdir
## umask
## deal with fd
below both output
守护进程继承了shell的屏幕和键盘输入.
pid_t pid= fork();
if (pid != 0) {
	system("sleep 3");
	printf("parent exit\n");
} else {
	printf("child start\n");
	setsid(); 
	fprintf(stdout, "out child\n");
	fprintf(stderr, "err child\n");

	system("sleep 7");

	fprintf(stdout, "after out child\n");
	fprintf(stderr, "after err child\n");
	printf("child exit\n");
}
# Zombie process
## <defunct>
forked child not reaped by parent will hooked in process list.
if parent was killed and exit <defunct> will repaped.
<defunct>表明父活着, 但不收尸.
## Double fork for parent long live 
mirgate subchild to init, parent just reap child.

# FAQ
* thread_info
可以快速访问task_struct.

* swtch_stack
线程切换

# The player
* Interrupt -- irq,softirq
* Process -- user process, kernel thread(workqueue)

# Common concpets
userspace: A process executing its own code outside the kernel.

##What is context?
###process context/user context: 
The kernel executing on behalf of a particular process 
(ie. a system call or trap) or kernel thread. 
You can tell which process with the current macro.) 
Not to be confused with userspace. 
Can be interrupted by software or hardware interrupts.

* register, kernel task_struct, stack. user text stack heap
###interrupt context: 
* Firo thinks the key point is interrupt context is irrelevant to process context!
* Can not schedule!
* register, maybe interrupt/process stack.
###softirq context
in the context of whichever process happens to be running at the wrong time; 
that is the "randomly chosen victim" aspect that Thomas was talking about.

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

###Entries of preempt schdule, FIXME
1 After Interrupt return,preempt_count_irq.
2 cond_resched
3 preempt_count

###What is the PREEMPT_ACTIVE
* This sets a flag bit in the preemption counter that has such a large value
	that it is never affected by the regular preemption counter increments

* It indicates to the schedule function that scheduling was not 
	invoked in the normal way but as a result of a kernel preemption.

* This ensures that the next task is selected as quickly as possible without 
	the hassle of deactivating the current one. If a high-priority task is waiting 
	to be scheduled, it will be picked by the scheduler class and will be allowed to run.

###preempt_count
* preempt 8
* softirq 8
* irq	4
* preempt active 1
* nmi 1


#Interrupt
## init
native_init_IRQ()->  set_intr_gate(i, irq_entries_start...
irq_entries_start define in arch/x86/kernel/entry_32.S

##Interrupt Context terminology
* in_irq
in __irq_enter I found preempt_count_add(HARDIRQ_OFFSET) only in here!
+irq disable 部设计preempt_cout

* in_softirq
should rename to in_softirq_or_bh_disable()
local_bh_disable use SOFTIRQ_DISABLE_OFFSET  (2 * SOFTIRQ_OFFSET)
__do_softirq use __local_bh_disable_ip(_RET_IP_, SOFTIRQ_OFFSET), damn it!

* in_interrupt
一种是irq路径上的do_softirq, 即in_serving_softirq
+local_bh_disbale()
//not in softirq context!!!
+local_bh_enable()
硬中断呢?
do_IRQ,肯定是.
local_irq_disable,不是, 但是no operation with preempt_count() 这种情况x86 in_interrupt是无感的.use irqs_disabled to check
[Question about in_interrupt() semantics with regard to softirqs]()

[Deal PF_MEMALLOC in softirq](http://thread.gmane.org/gmane.linux.kernel/1152658)

##softirq
同一个softirq可以在不同的CPU上同时运行，softirq必须是可重入的。
* not allow execute nest but can recusive lock:local_bh_disable 
current->preemt_count + SOFIRQ_OFFSET also disable preempt current process.
* hardirq on, can't sleep
* not percpu
## tasklet and kernel timer is based on softirq
新增softirq, 是要重新编译内核的, 试试tasklet也不错.
.不允许两个两个相同类型的tasklet同时执行，即使在不同的处理器上
* First of all, it's a conglomerate of mostly unrelated jobs, 
 which run in the context of a randomly chosen victim 
 w/o the ability to put any control on them. --Thomas Gleixner

tasklet different with other softirq is run  signal cpu core
spinlock_bh wider then spinlock 

###time of softirq
* follow hardirq, irq_exit()
* re-enables softirq, local_bh_enable/spin_unlock_bh(); explicity checks executes, netstack/blockIO.
* ksoftirqd

###tasklet
tasklet like a workqueue, sofirq like kthread. that is wonderful, does it?
tasklet 被__tasklet_schedule到某个cpu的percu 变量tasklet_vec.tail上保证了
只有一个cpu执行同一时刻.

#Process
##kthread and workqueue
kthread do specific func onshot
workqueue can dynamic set different specific work
workqueue is just a coat of kthread

## Differences between softirq and workqueue
Softirq(tasklet)
init static:	DECLARE_TASKLET(), DECLARE_TASKLET_DISABLED()
init dynamic:	tasklet_init()
schedule:	tasklet_schedule()
exit:		tasklet_kill()
pros and cons:	execute in interrupt context, serialization, tasklet blonged to the cpu which tasklet_schedule() it, oneshot

Workqueue
init static:	DECLARE_WORK()
init dynamic:	kzalloc() INIT_WORK(), kzalloc() PREPARE_WORK()
schedule1:	create_singlethread_workqueue(), create_workqueue()  
		queue_work()
exit1:		flush_workqueue(), destroy_workqueue()
schedule2:	schedule_work()
exit2:		(none)
pros and cons:	execute in process context, workqueue has two type, driver can create workqueue or use system workqueue, oneshot

#FAQ
##When to save irq rather than just disable irq
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

##what about irq nested?
http://lwn.net/Articles/380937/

##irq and lock
irq and lock are complete different things!
when need to disable irq just get rid of mess of shared data.


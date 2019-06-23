---
tags: [ cs ] 
title: The computing processes
date: 2015-02-27T15:46:12+08:00 
category: cs
---


# The stack / Call stack
https://en.wikipedia.org/wiki/Call_stack#Functions_of_the_call_stack
https://dl.acm.org/citation.cfm?id=2716018#.W5zV9yqRi-c.twitter
task_struct->stack:task_stack_page
task_struct->thread.sp: top of stack

# PID
[PIDTYPE_PID](https://lwn.net/Articles/606925/)

# Processes
## Instructions
opcode, 
registers and RAM
## Subroutine and procedure
a group of instructions
stack
## interrupt handler
procdure + optional own stack
## softirq handler
procdure + optional own stack
## kernel thread
[thread vs event](http://courses.cs.vt.edu/cs5204/fall09-kafura/Presentations/Threads-VS-Events.pdf)
change of processes: associate change structure with a object and has more states; event-driven -> schedule
space: +kernel address space, more kind of resource space
change: implict changings.
## process(horizantal)
change: linkage of user space code
space: whole address space
## thread
share process address space
tls
[Linux线程TLS（Thread-Local Storage）相关 GS段寄存器作用](http://www.maxwellxxx.com/TLS)
## usage
https://www.quora.com/What-is-the-relation-between-the-number-of-threads-and-the-number-of-CPU-cores

# Change of processes
## Instructions
pc + 1
## Subroutine and procedure
save: push pc + 1; optional caller saved registers 
entry: save rbp; callee saved registers
return: mov rbp -> rsp; pop rbp; ret
## softirq

## signal

## interrupt
[history of interrupts](https://people.cs.clemson.edu/~mark/interrupts.html)
[Another History of interrupts with video](https://virtualirfan.com/history-of-interrupts)
Interrupts: asynonymous(passively received), external
Exception: synonymous(actively detected), internal
Software interrupts: is a trap. int/int3, into, bound.
IPI:
[为什么Linux内核不允许在中断中休眠](http://wangcong.org/2012/06/01/-e4-b8-ba-e4-bb-80-e4-b9-88linux-e5-86-85-e6-a0-b8-e4-b8-8d-e5-85-81-e8-ae-b8-e5-9c-a8-e4-b8-ad-e6-96-ad-e4-b8-ad-e4-bc-91-e7-9c-a0-ef-bc-9f/)
### Hardlockup
www.kernel.org/doc/Documentation/lockup-watchdogs.txt
### Mask exception
RF in EFLAGS for masking #DB
SS & SP

## exception

## Thread switch
[Al Viro's new execve/kernel_thread design](https://lwn.net/Articles/520227/)
### call+jump+ret - 0100301bfdf56a2a370c7157b5ab0fbf9313e1cd
((last) = __switch_to_asm((prev), (next)));				#=====> call

jmp     __switch_to							#=====> jmp + ret

### Old version switch_to - push+jmp+ret
switch_to
-       asm volatile("pushl %%ebp\n\t"          /* save    EBP   */     \
-                    "movl %%esp,%[prev_sp]\n\t"        /* save    ESP   */ \	#=====>PREV: Save ESP into task struct thread.
-                    "movl %[next_sp],%%esp\n\t"        /* restore ESP   */ \	#=====>NEXT: Setup stack for linkage from task struct thread.
-                    "movl $1f,%[prev_ip]\n\t"  /* save    EIP   */     \	#=====>PREV: Save PC into task sturct thread.
-                    "pushl %[next_ip]\n\t"     /* restore EIP   */     \	#=====>NEXT: push - Store PC on stack from task struct thread;
-                    "jmp __switch_to\n"        /* regparm call  */     \	#=====>NEXT: jmp + ret - Return and restore PC. 
-                    "1:\t"                                             \
-                    "popl %%ebp\n\t"           /* restore EBP   */     \
-                                                                       \
-                    /* output parameters */                            \
-                    : [prev_sp] "=m" (prev->thread.sp),                \
-                      [prev_ip] "=m" (prev->thread.ip),                \
-                      "=a" (last),                                     \
-                                                                       \
-                      /* clobbered output registers: */                \
-                      "=b" (ebx), "=c" (ecx), "=d" (edx),              \
-                      "=S" (esi), "=D" (edi)                           \
-                      /* input parameters: */                          \
-                    : [next_sp]  "m" (next->thread.sp),                \
-                      [next_ip]  "m" (next->thread.ip),                \
[Why does switch_to use push+jmp+ret to change EIP, instead of jmp directly?](https://stackoverflow.com/questions/15019986/why-does-switch-to-use-pushjmpret-to-change-eip-instead-of-jmp-directly/15024312)

## Kernel thread
kernel_thread->kthread->do_exit

## Task switching
https://lkml.org/lkml/2016/3/21/517
### DBG: Softlockup
 ps aux | grep watchdog
root        13  0.0  0.0      0     0 ?        S    08:23   0:00 [watchdog/0]
root        16  0.0  0.0      0     0 ?        S    08:23   0:00 [watchdog/1]
root        22  0.0  0.0      0     0 ?        S    08:23   0:00 [watchdog/2]
root        28  0.0  0.0      0     0 ?        S    08:23   0:00 [watchdog/3]
### DBG: Hung tasks bugs
#### think for myself
A kernel bug casuse task to be stuck in "D" state indefinitely.
1. A D state task wait list. 
2. Hung task timeout.
3. Timestamp on adding task to "D" state wait list.
4. Kernel thread for detecting hung tasks - schedule timeout; why kthread?
##### Problems
How could I find all the D state tasks?
1. kernel must use specific functions to put D-task on wait list.
2. Embeded codes into specific functions to catch 'D' state tasks and put them in the wait list for Hung tasks detecting.
#### Kernel Implemention
DETECT_HUNG_TASK
kernel/hung_task.c
check_hung_uninterruptible_tasks
* diffeneces
1: kenrel task list - init_task.tasks and p->signal->thread_head in copy_process
3: t->nvcsw + t->nivcsw, t->last_switch_count and timeout
cat /proc/self/status | grep ctxt_switches

### Preemption
#### User preemption - Linux kernel user mode is always User preemption.
* When returning to user-space from a system call.
syscall_return_slowpath
* When returning to user-space from an interrupt hander.
retint_user->prepare_exit_to_usermode
#### Linux kernel kernel mode is coppertive when CONFIG_PREEMPT is not set.
bloked (which results in a call to schedule())
If a task in the kernel explicitly calls schedule() it's involuntary!!!
#### Linux kernel kernel mode is coppertive + preemptive when CONFIG_PREEMPT is set.
* When an interrupt handler exits, before returning to kernel-space.
retint_kernel->preempt_schedule_irq->cond_resched
* need_resched - When kernel code becomes preemptible again.
1. set_tsk_need_resched() in resched_curr
tick: check_preempt_tick or entity_tick
fork: wake_up_new_task->check_preempt_curr->check_preempt_wakeup
wakeup: check_preempt_wakeup
...
2. if (need_resched()) cond_resched();
#### LQO
* if (!preempt && prev->state)in __schedule why prev->state? 
it's because of need_resched
?? schedule_idle

# LQO
* canary
https://my.oschina.net/macwe/blog/610357
boot_init_stack_canary
* bug??
__do_softirq
258         pending = local_softirq_pending();
259         account_irq_enter_time(current);
260 
261         __local_bh_disable_ip(_RET_IP_, SOFTIRQ_OFFSET);
* os presentations
http://courses.cs.vt.edu/cs5204/fall09-kafura/Presentations/

# Procedure and subroutine and linkage method
https://people.cs.clemson.edu/~mark/subroutines.html
https://en.wikipedia.org/wiki/Subroutine
* 1945 Turing on subroutines in [Proposed electronic calculator.](http://www.alanturing.net/turing_archive/archive/p/p01/P01-011.html)
In Chapter 6. Outline of Logical Control.
We also wish to be able to arrange for the splitting up of operations into...
When we wish to start on a subsidiary operation we need only make a note
of where we left off the major operation...
* 1952 [The use of sub-routines in programmes](http://www.laputan.org/pub/papers/wheeler.pdf)
The above remarks may be summarized by saying sub-routines are very useful — although not absolutely necessary — and that the prime objectives to be born in mind when constructing them are simplicity of use, correctness of codes and accuracy of description. All complexities should—if possible—be buried out of sight.
* 1960 [Dijkstra, E. W. (1960). "Recursive Programming"](https://link.springer.com/article/10.1007%2FBF01386232)

## linkage
https://en.wikipedia.org/wiki/Link_register


# Structred programming
Structured Programming - Dijkstra

# kthread
## resource 

## Change
Periodical: schedule_timeout
Demand: 

## inter-process info

# Fork a new process 
What does the child process need from parent?
	sched_fork: setup schduling stuff
	memory: copy parent's mm
How to share memory stuff with parent process?
	Linux use COW technique to do this.
	How does COW work?
	Why does Linux just share page?
How to diverge the child execution flow from parent?
	what is the first instruction executed by the child process?
# Insepct process status
do_task_stat
Kernel mapping: tgid_base_stuff show_map_vma
# Protection 
3A: Chaper 5
# FAQ
Check glibc sysdeps/unix/sysv/linux/x86_64/clone.S for creating a new thread.
## idle kernel stack
master idle进程的kernel stack在init/init_task.c:init_thread_union
其他进程的kernel stack是fork产生.
this_cpu_write(kernel_stack,(unsigned long)task_stack_page(next_p) +THREAD_SIZE);
this_cpu_write(cpu_current_top_of_stack,(unsigned long)task_stack_page(next_p) +THREAD_SIZE);
主处理器上的idle由原始进程(pid=0)演变而来。从处理器上的idle由init进程fork得到，但是它们的pid都为0 init_idle.
Deamonize
# Zombie process
## <defunct>
forked child not reaped by parent will hooked in process list.
if parent was killed and exit <defunct> will repaped.
<defunct>表明父活着, 但不收尸.


---
tags: [ cs ] 
title: The computing processes
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# Formal cause
[thread vs event](http://courses.cs.vt.edu/cs5204/fall09-kafura/Presentations/Threads-VS-Events.pdf)
## PID
[PIDTYPE_PID](https://lwn.net/Articles/606925/)
task->pid: thread id: unique
task->tgid: task group id: child thread share parent's tgid
Check copy_process

# kthread - Kernel thread
[active_mm](https://www.kernel.org/doc/Documentation/vm/active_mm.txt)
kernel_thead->kthread->do_exit
#define PF_KTHREAD              0x00200000
#define PF_KSWAPD               0x00020000
#define PF_IDLE                 0x00000002      /* I am an IDLE thread */
#define PF_VCPU                 0x00000010      /* I'm a virtual CPU */
#define PF_WQ_WORKER            0x00000020      /* I'm a workqueue worker */
PID: 197    TASK: ffff8bc8dd350480  CPU: 0   COMMAND: "kswapd0"
  flags = 0xa20840,
PID: 19     TASK: ffff8bc214160780  CPU: 1   COMMAND: "kworker/1:0H"
  flags = 0x4208060,
crash> task -R flags
PID: 0      TASK: ffffffff880134c0  CPU: 0   COMMAND: "swapper/0"
  flags = 0x200102,
PID: 0      TASK: ffff8bc21476c500  CPU: 14  COMMAND: "swapper/14"
  flags = 0x200042,
## task => kthread
to_kthread and vfork_done
commit 63706172f332fd3f6e7458ebfb35fa6de9c21dc5
Author: Oleg Nesterov <oleg@redhat.com>
Date:   Wed Jun 17 16:27:45 2009 -0700
## kthread and worker
kthread->data
## worker is rescuer?
current_is_workqueue_rescuer and worker->rescue_wq

# Insepct process status
do_task_stat
Kernel mapping: tgid_base_stuff show_map_vma
# Protection 
3A: Chaper 5
## idle kernel stack
master idle进程的kernel stack在init/init_task.c:init_thread_union
this_cpu_write(kernel_stack,(unsigned long)task_stack_page(next_p) +THREAD_SIZE);
this_cpu_write(cpu_current_top_of_stack,(unsigned long)task_stack_page(next_p) +THREAD_SIZE);
主处理器上的idle由原始进程(pid=0)演变而来。从处理器上的idle由init进程fork得到，但是它们的pid都为0 init_idle.
# Zombie process <defunct>
forked child not reaped by parent will hooked in process list.
if parent was killed and exit <defunct> will repaped.

# Context switch
[Evolution of the x86 context switch in Linux](https://www.maizure.org/projects/evolution_x86_context_switch_linux/index.html)
[Al Viro's new execve/kernel_thread design](https://lwn.net/Articles/520227/)
## call+jump+ret - 0100301bfdf56a2a370c7157b5ab0fbf9313e1cd
((last) = __switch_to_asm((prev), (next)));                             #=====> call
jmp     __switch_to                                                     #=====> jmp + ret
[Why does switch_to use push+jmp+ret to change EIP, instead of jmp directly?](https://stackoverflow.com/questions/15019986/why-does-switch-to-use-pushjmpret-to-change-eip-instead-of-jmp-directly/15024312)

# Wait example
 #6 [ffff883f15637c90] proc_evict_inode at ffffffff812696fd
 #7 [ffff883f15637ca0] evict at ffffffff8121cded
 #8 [ffff883f15637cc0] __dentry_kill at ffffffff812194b6
 #9 [ffff883f15637ce0] shrink_dentry_list at ffffffff8121a0c0
#10 [ffff883f15637d10] d_invalidate at ffffffff8121a8c8
#11 [ffff883f15637d50] proc_flush_task at ffffffff8126e609
#12 [ffff883f15637dc0] release_task at ffffffff81081230
                        # wait_task_zombie
#13 [ffff883f15637e18] wait_consider_task at ffffffff81081c19
#14 [ffff883f15637e80] do_wait at ffffffff8108226d

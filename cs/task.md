---
tags: [ cs ] 
title: The computing process
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# Timestamp
se.sum_exec_runtime                          :             0.036973
se.avg.last_update_time                      :           1884199936

# Pid
[Namespaces in operation, part 3: PID namespaces](https://lwn.net/Articles/531419/)
[PID, PGID, SID, PIDTYPE_PID](https://lwn.net/Articles/606925/)
task->pid: thread id: unique
task->tgid: task group id: child thread share parent's tgid
Check copy_process

# stack management
[Expanding the kernel stack](https://lwn.net/Articles/600644/)
[Virtually mapped kernel stacks](https://lwn.net/Articles/692208/)

Related code:
task_struct->stack:task_stack_page
task_struct->thread.sp: top of stack
kernel_stack, cpu_current_top_of_stack
## x86 TSS and sp0
[x86/entry/64: Remove thread_struct::sp0](https://lore.kernel.org/patchwork/patch/828764/):d375cf1530595e33961a8844192cddab913650e3
GDT -> TSS descriptor -> TSS 
tss_struct cpu_tss, ltr, str 
TSS: x86_hw_tss v3a: 7.7 
### Onset
arch/x86/include/asm/desc.h
DECLARE_PER_CPU_PAGE_ALIGNED(struct gdt_page, gdt_page);        #===> GDT
arch/x86/kernel/process.c
DEFINE_PER_CPU_SHARED_ALIGNED(struct tss_struct, cpu_tss)       #===> tss_struct
start_kernel ... ->start_secondary and trap_init->
cpu_init
        t->x86_tss.io_bitmap_base = offsetof(struct tss_struct, io_bitmap);
        load_sp0(t, &current->thread);  #===> tss->x86_tss.sp0 = thread->sp0
        set_tss_desc(cpu, t);           #===> set tss descriptor to GDT; tss descriptor.base = tss_struct
        load_TR_desc();                 #===> ltr
_do_fork -> copy_process
        dup_task_struct
                stack = alloc_thread_stack_node(tsk, node);             # ===> THREAD_SIZE 2 pages.
                tsk->stack = stack;
        copy_thread_tls
                # arch/x86/include/asm/processor.h
                # #define task_pt_regs(tsk)       ((struct pt_regs *)(tsk)->thread.sp0 - 1)
                p->thread.sp0 = (unsigned long)task_stack_page(p) + THREAD_SIZE;        # task->stack; THREAD_SIZE 2 pages;
                childregs = task_pt_regs(p);
                # struct fork_frame {
                #       struct inactive_task_frame frame;
                #       struct pt_regs regs;
                # };
                fork_frame = container_of(childregs, struct fork_frame, regs);
                p->thread.sp = (unsigned long) fork_frame;
                *childregs = *current_pt_regs();
        # Add child task to run_queue. Then scheded.
__switch_to -> load_sp0(tss, next);
### nuclus
entry_SYSCALL_64 -> movq    PER_CPU_VAR(cpu_current_top_of_stack), %rsp

## idle threads and their kernel stack
Boot processor's idle thread kernel stack is init/init_task.c:init_thread_union; init_task is pid 0;
Application processor's idle thread are forked in idle_init; pids are both 0. 

# kthread
[active_mm](https://www.kernel.org/doc/Documentation/vm/active_mm.txt)
kernel_thead->kthread->do_exit
PF_KTHREAD              0x00200000
PF_KSWAPD               0x00020000
PF_IDLE                 0x00000002      /* I am an IDLE thread */
PF_VCPU                 0x00000010      /* I'm a virtual CPU */
PF_WQ_WORKER            0x00000020      /* I'm a workqueue worker */
PID: 197    TASK: ffff8bc8dd350480  CPU: 0   COMMAND: "kswapd0"
  flags = 0xa20840,
PID: 19     TASK: ffff8bc214160780  CPU: 1   COMMAND: "kworker/1:0H"
  flags = 0x4208060,
crash> task -R flags
PID: 0      TASK: ffffffff880134c0  CPU: 0   COMMAND: "swapper/0"
  flags = 0x200102,
PID: 0      TASK: ffff8bc21476c500  CPU: 14  COMMAND: "swapper/14"
  flags = 0x200042,
## task to kthread
to_kthread and vfork_done
commit 63706172f332fd3f6e7458ebfb35fa6de9c21dc5
Author: Oleg Nesterov <oleg@redhat.com>
Date:   Wed Jun 17 16:27:45 2009 -0700
kthreads: rework kthread_stop()

# exit 
Zombie process <defunct>
forked child not reaped by parent will hooked in process list.
if parent was killed and exit <defunct> will repaped.

# wait
proc_evict_inode at ffffffff812696fd
evict at ffffffff8121cded
__dentry_kill at ffffffff812194b6
shrink_dentry_list at ffffffff8121a0c0
d_invalidate at ffffffff8121a8c8
proc_flush_task at ffffffff8126e609
release_task at ffffffff81081230  # wait_task_zombie
wait_consider_task at ffffffff81081c19
do_wait at ffffffff8108226d

# Observation
do_task_stat
Kernel mapping: tgid_base_stuff show_map_vma

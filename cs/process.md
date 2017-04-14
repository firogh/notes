---
tags: [ cs ] 
title: The computational process
date: 2015-02-27T15:46:12+08:00 
category: cs
---

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



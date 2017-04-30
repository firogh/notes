---
tags: [ kernel ] 
title: The definitive guide to Linux x86 entry.S
date: 2017-04-26T21:39:41+08:00 
category: cs
---

Documentation/x86/entry_64.txt
[对Linux x86-64架构上硬中断的重新认识](http://www.lenky.info/archives/2013/03/2245)
[The Definitive Guide to Linux System Calls](https://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls/)
[选择内存屏障指令|alternative](http://www.groad.net/bbs/thread-3276-1-1.html)


fs and gs register
For using in TLS, please check [OSDEV TLS](http://wiki.osdev.org/Thread_Local_Storage#ABI)

#ifdef CONFIG_X86_64
#define __percpu_seg            gs
#define __percpu_mov_op         movq
#else
#define __percpu_seg            fs
#define __percpu_mov_op         movl
#endif

Steps to handle intterrupt

For logical address to linear address, see intel SDM v3a 3.4 LOGICAL AND LINEAR ADDRESSES.
For stack switching during escalate the CPL, see SDM v3a 5.8.5 stack switching. The processor will automatically chose the espCPL stack to use during changing in privilege level.
For more details on stack switching, please check the Figure 5-13. Stack Switching During an Interprivilege-Level Call
For fast system call, check 3a 5.8.7 Performing Fast Calls to System Procedures
For TSS and TR, check 3a 7.2
For Linux hanld irq processes, check ULK 3rd Chapter 4: Hardware Handling of Interrupts and Exceptions

# For syscall
Common path for syscalls
glibc -> AT_SYSINFO-> __kernel_vsyscall -> sysenter/syscall/in0x80
just for vDSO syscalls
glibc -> AT_SYSINFO_EHDR-> vDSO elf

---
tags: [ kernel ] 
title: The definitive guide to Linux x86 entry.S
date: 2017-04-26T21:39:41+08:00 
category: cs
---

Definitive? Actually, it's not and going to die.

# ref
https://groups.google.com/forum/#!topic/comp.arch/CjDs4MJCBow%5B1-25%5D

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

# system calls
[The Definitive Guide to Linux System Calls](https://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls/)
[SYSENTER/SYSEXIT vs.SYSCALL/SYSRET](https://groups.google.com/forum/#!topic/comp.arch/CjDs4MJCBow%5B1-25%5D)
[Measurements of system call performance and overhead](http://arkanis.de/weblog/2017-01-05-measurements-of-system-call-performance-and-overhead)
[AMD vs Intel and syscall vs sysenter](https://reverseengineering.stackexchange.com/a/16511/16996)
[System Call Optimization with the SYSENTER Instruction](https://www.codeguru.com/cpp/misc/misc/system/article.php/c8223/System-Call-Optimization-with-the-SYSENTER-Instruction.htm)
[Sysenter Based System Call Mechanism in Linux 2.6](http://articles.manugarg.com/systemcallinlinux2_6.html)
[kernel documentation](https://www.kernel.org/doc/Documentation/x86/entry_64.txt)
Meltdown and Spectre
### syscall
* save: pc + 1, old rsp, registers
pc + 1-> RCX
* entry: pc
IA32_LSTAR -> pc
### kernel implementations
64-bit long mode: syscall; check syscall_init
64-bit compatible kernel: sysenter, syscall, or int 0x80; check __kernel_vsyscall and def_idts
??32-bit kernel: int 0x80, sysenter;
### vDSO and vsyscall
[On vsyscalls and the vDSO](https://lwn.net/Articles/446528/)
[linux syscalls on x86 64](http://blog.tinola.com/?e=5)
### Example
* 64-bit without COMPAT_32_/compatible kernel
./int80
[  730.583700] traps: int80[1697] general protection ip:4000c4 sp:7ffd84b59730 error:402 in int80[400000+1000]
Segmentation fault (core dumped)
* 64-bit syscall
syscall.S

## rcx and r10
Check x86_64 ABI: Linux conventions and 
According to [x86 syscall instruction](https://www.felixcloutier.com/x86/syscall), rcx is used to passing next rip.
According to entry_SYSCALL_64, rcx is rip before it is pushed on the kernel stack.
So r10 is right 4th args passed from userspace.
According to do_syscall_64,
                regs->ax = sys_call_table[nr](
                        regs->di, regs->si, regs->dx,
                        regs->r10, regs->r8, regs->r9);


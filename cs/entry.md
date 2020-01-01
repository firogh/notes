---
tags: [ kernel ] 
title: The definitive guide to Linux x86 entries
date: 2017-04-26T21:39:41+08:00 
category: cs
---

# All entries
[Documentation/x86/entry_64.txt](https://www.kernel.org/doc/Documentation/x86/entry_64.txt)

# Entry irq
[对Linux x86-64架构上硬中断的重新认识](http://www.lenky.info/archives/2013/03/2245)
## Steps to handle intterrupt
For logical address to linear address, see intel SDM v3a 3.4 LOGICAL AND LINEAR ADDRESSES.
For stack switching during escalate the CPL, see SDM v3a 5.8.5 stack switching. The processor will automatically chose the espCPL stack to use during changing in privilege level.
For more details on stack switching, please check the Figure 5-13. Stack Switching During an Interprivilege-Level Call
For fast system call, check 3a 5.8.7 Performing Fast Calls to System Procedures
For TSS and TR, check 3a 7.2
For Linux hanld irq processes, check ULK 3rd Chapter 4: Hardware Handling of Interrupts and Exceptions

# Entry exception
## paranoid_entry
Check Documentation/x86/entry_64.txt
## error_entry
tglx: commit 0457d99a336be658cea1a5bdb689de5adb3b382d
Author:     Andi Kleen <ak@muc.de>
AuthorDate: Tue Feb 12 20:17:35 2002 -0800
Commit:     Linus Torvalds <torvalds@home.transmeta.com>
CommitDate: Tue Feb 12 20:17:35 2002 -0800
    [PATCH] x86_64 merge: arch + asm


# Entry system calls
[The Definitive Guide to Linux System Calls](https://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls/)

## sysenter vs syscall
[SYSENTER/SYSEXIT vs.SYSCALL/SYSRET](https://groups.google.com/forum/#!topic/comp.arch/CjDs4MJCBow%5B1-25%5D)
[Measurements of system call performance and overhead](http://arkanis.de/weblog/2017-01-05-measurements-of-system-call-performance-and-overhead)
[AMD vs Intel and syscall vs sysenter](https://reverseengineering.stackexchange.com/a/16511/16996)
[System Call Optimization with the SYSENTER Instruction](https://www.codeguru.com/cpp/misc/misc/system/article.php/c8223/System-Call-Optimization-with-the-SYSENTER-Instruction.htm)
[Sysenter Based System Call Mechanism in Linux 2.6](http://articles.manugarg.com/systemcallinlinux2_6.html)

## system call restart mechanism and ORIG_EAX
[A new system call restart mechanism](https://lwn.net/Articles/17744/)
[Why set ORIG_EAX(%esp) to -1 in arch/i386/kernel/entry.S:error_code?](https://lkml.org/lkml/2006/8/29/350)

## kernel implementations
arch/x86/include/asm/proto.h
64-bit long mode: syscall; check syscall_init
64-bit compatible kernel: sysenter, syscall, or int 0x80; check __kernel_vsyscall and def_idts
32-bit kernel: int 0x80, sysenter;
### 64-bit without COMPAT_32/compatible kernel support
./int80
[  730.583700] traps: int80[1697] general protection ip:4000c4 sp:7ffd84b59730 error:402 in int80[400000+1000]
Segmentation fault (core dumped)

## x86_64 rcx and r10
Check x86_64 ABI: Linux conventions and  according to [x86 syscall instruction](https://www.felixcloutier.com/x86/syscall), rcx is used to passing next rip.
According to entry_SYSCALL_64, rcx is rip before it is pushed on the kernel stack. So r10 is right 4th args passed from userspace.
According to do_syscall_64, regs->ax = sys_call_table[nr](regs->di, regs->si, regs->dx, regs->r10, regs->r8, regs->r9);

## x86_32 asmlinkage
[By default gcc passes parameters on the stack for x86-32 arch, so what is it needed for? It's because linux kernel uses -mregparm=3 option which overrides the default behaviour](https://qr.ae/Ti5MJJ)
[enbaled -mregparm=3 Shrinking the kernel with gcc](https://lwn.net/Articles/67175/)
[What is asmlinkage?](https://kernelnewbies.org/FAQ/asmlinkage)
However, for C functions invoked from assembly code, we should explicitly declare the function's calling convention, because the parameter passing code in assembly side has been fixed. Show all predefined macros for your compiler

## vDSO
[Creating a vDSO: the Colonel's Other Chicken](http://www.linuxjournal.com/content/creating-vdso-colonels-other-chicken?page=0,0)
[What is linux-gate.so.1](http://www.trilithium.com/johan/2005/08/linux-gate/)
glibc -> AT_SYSINFO-> __kernel_vsyscall -> sysenter/syscall/in0x80
just for vDSO syscalls
glibc -> AT_SYSINFO_EHDR-> vDSO elf 
[On vsyscalls and the vDSO](https://lwn.net/Articles/446528/)
[linux syscalls on x86 64](http://blog.tinola.com/?e=5)

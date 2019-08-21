---
tags: [ cs ] 
title: System booting
date: 2015-11-12
category: cs
---

# kernel boot process
Documentation/x86/boot.txt
[The Kernel Boot Process](https://manybutfinite.com/post/kernel-boot-process/)
arch/x86/boot/header.S::start_of_setup
arch/x86/boot/main.c::main()
	arch/x86/boot/memory.c::detect_memory()
	arch/x86/boot/memory.c::detect_memory_e820() = boot_params.e820_entries
	arch/x86/boot/pm.c::go_to_protected_mode()
arch/x86/boot/pmjump.S::protected_mode_jump
arch/x86/kernel/compressed/head_64.S::startup_32
arch/x86/kernel/compressed/head_64.S::startup_64
arch/x86/kernel/head_64.S::startup_64
kernel/main.c::start_kernel()
## Reloctaion for decompress
974f221c84b05b1dc2f5ea50dc16d2a9d1e95eda
x86/boot: Move compressed kernel to the end of the decompression buffer
## Horrable commit description on phys_base
commit 1ab60e0f72f71ec54831e525a3e1154f1c092408
Author: Vivek Goyal <vgoyal@in.ibm.com>
Date:   Wed May 2 19:27:07 2007 +0200
    [PATCH] x86-64: Relocatable Kernel Support

# SMP boot
Check SDM v3 BSP and AP Processors
BSP - boot strap processor； AP - application processor
## Core commit
setup_real_mode-> trampoline_header->start = (u64) secondary_startup_64;
commit f37240f16bec91f15ce564515f70a6ca9715ce96
Author: Jarkko Sakkinen <jarkko.sakkinen@intel.com>
Date:   Tue May 8 21:22:43 2012 +0300
    x86, realmode: header for trampoline code
## BSP
### Build time for real_mode_header in arch/x86/realmode/rm/header.S
In pasyms.h, git gud!
pa_trampoline_header = trampoline_header;
pa_trampoline_start = trampoline_start;
pa_startup_32 = startup_32;
pa_startup_64 = startup_64;
### Early init
setup_real_mode-> trampoline_header->start = (u64) secondary_startup_64;  # tr_start
### start_kernel
kernel_init->smp_init->cpu_up->do_cpu_up->_cpu_up->
ap hp threadfn -> bringup_cpu -> __cpu_up -> smp_ops.cpu_up(cpu, tidle) is native_cpu_up
        do_boot_cpu is the core function. It set up the code for APs to run and check cpu_callin_mask.
        start_eip = real_mode_header->trampoline_start;
        initial_code = (unsigned long)start_secondary                   # initial_code
## AP
trampoline_start -> ... -> startup_64 -> tr_start(%rip) is secondary_startup_64 -> initial_code(%rip) is start_secondary
-> cpu_init

# Normalize %cs
程序的起始地址在链接脚本中被设置为 0，如果 setup 被加载到其他地方(起始地
址不为 0 的地方)，那么指令里面访问的全局符号都会有重定位的问题。由于 Boot Loader
跳转到上面这段代码的时候，把 DS 设置为 setup 加载的基地址，而第 17 行访问_end 默认
是用数据段寄存器 DS 的，所以不会有重定位的问题。但是在 38 行转移指令用的 CS 寄存器，
而符号 main 的地址是在链接期决定的，现在加载的基地址改变了，那肯定会出问题了。所
以事先在第 24 行把 CS 设置为和 DS 一样，这样就解决了重定位的问题了。代码的最后跳转
到了 arch/x86/boot/main.c 中的 main 函数中继续执行。

# print with int $0x10
https://stackoverflow.com/questions/38041478/int-10h-not-working-in-qemu

# A20
[Why enable A20 line in Protected Mode?][https://stackoverflow.com/questions/33827474/why-enable-a20-line-in-protected-mode]
This gate, (the A20 gate), is controlled by a GPIO pin on the keyboard controller IC. Thus, you need to enable it before going into protected mode. If you didn't, (and say you flat-mapped all 4GB of physical memory), then as Micheal Petch indicated, "every odd numbered megabyte region will be inaccessible. So 1mb-2mb will actually reference 0-1mb, 3mb-4mb will reference 2mb-3mb etc." See also:


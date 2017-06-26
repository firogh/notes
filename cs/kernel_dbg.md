---
tags: [ cs, debug, kernel ] 
title: Kernel debugging primer
date: 2017-04-05T14:45:21+08:00 
category: cs
---

[1]: http://vegardno.blogspot.com/2016/08/sync-debug.html?m=1

# Dynamic debug 
Precondition: CONFIG_DYNAMIC_DEBUG
How to use it during kernel booting?
Use boot option:
dyndbg="file drivers/usb/host/ehci-hcd.c +p; file 
drivers/usb/host/ehci-ppc-of.c +p" loglevel=8
loglevel=8 dyndbg="module ehci_pci +p; 
module ehci_hcd +p; module usbcore +p"
After booting:
/sys/kernel/debug/dynamic_debug/control

# Dev print functions
drivers/base/core.c
define_dev_printk_level 
THese functions are like pr_func

# oops
Kernel oops relates to invalid memory access, including sigev and sigbus.
[kernel oops tracing](https://www.kernel.org/doc/Documentation/oops-tracing.txt)
[Debugging a kernel crash found by syzkaller][1]



# lockup
phenomenon: LOCKUP_DETECTOR
[kernle doc - Softlockup detector and hardlockup detector](https://www.kernel.org/doc/Documentation/lockup-watchdogs.txt)
[Debugging Linux Kernel Lockup / Panic / Oops](http://www.av8n.com/computer/htm/kernel-lockup.htm)
Causes: deadlock, hardware, irqoff and loop

# hung task
phenomenon: DETECT_HUNG_TASK
Includes the interruptable task.
Causes: deadlock, race condition

# kprobes

# deadlock
Lockdep
[The kernel lock validator](https://lwn.net/Articles/185666/)
[Runtime locking correctness validator](https://www.kernel.org/doc/Documentation/locking/lockdep-design.txt)
https://lkml.org/lkml/2013/2/4/4
http://bbs.chinaunix.net/thread-4183696-1-1.html

# kasan
setup_arch->kasan_init
[KernelAddressSanitizer a fast memory error detector for the Linux kernel](http://events.linuxfoundation.org/sites/events/files/slides/LinuxCon%20North%20America%202015%20KernelAddressSanitizer.pdf)
[kasan found stack out of bounds](https://github.com/zfsonlinux/zfs/pull/4708/commits/01709937be3c28a89eff83e0e657a72826947506)
[lwn The kernel address sanitizer](https://lwn.net/Articles/612153/)
[out of bounds](https://lkml.org/lkml/2014/12/3/128)

# CONFIG_DEBUG_PAGEALLOC
check_poison_mem in alloc_pages
free_pages_prepare posion



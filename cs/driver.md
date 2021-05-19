---
tags: [ kernel ] 
title: Linux device driver
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# Modversions
https://lwn.net/Articles/21393/
https://lwn.net/Articles/707520/
## KABI 
Ludicrously Technical – Kernel ABI Tracking: http://www.jonmasters.org/blog/2007/06/16/ludicrously-technical-kernel-abi-tracking/

# Kernel module programming
sudo depmod -n -a -F /boot/System.map-4.3.5-300.fc23.x86_64  4.3.5-300.fc23.x86_64 > /tmp/m.log
CFLAGS_jc.o := -O2 
ccflags-y := -O2
## Mapping
[ioremap() and memremap()](https://lwn.net/Articles/653585/)
## kernel module param
include/asm-generic/vmlinux.lds.h
                KEEP(*(__param))                                        \
__module_param_call
__attribute__ ((unused,__section__ ("__param"),aligned(sizeof(void *)))) \
## Lib
devm_kfree and [devm_kfree does not need for fail case of probe function and for remove function.](http://lists.freedesktop.org/archives/dri-devel/2013-May/038943.html)

# Device model
The device model was originally intended to make power management tasks easier - LDD?
Documentation/driver-model
[Driver porting: Device model overview](http://lwn.net/Articles/31185/)
[Devices in /sys](https://lwn.net/Articles/604413/)

# History of module linkage and loader
## ksyms - removed
History: commit d80e0e9b2620f8ec6508d43484bd8300401d0383 (tag: 0.99.14d)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:09:16 2007 -0500
    Import 0.99.14d
+++ b/include/linux/module.h
+ * Dynamic loading of modules into the kernel.
+++ b/kernel/ksyms.S
+ * Herein lies all the functions/variables that are "exported" for linkage
+ * With dynamically loaded kernel modules. Could do with making this a bit 
+ * cleaner!
History: commit 75bcc1d5a30e1d2045c3564a668c11dbba5cfe3f (tag: 1.1.0)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:09:26 2007 -0500
    Import 1.1.0
convert ksyms.S to ksyms.c
tglx: commit ff08f6fe935ff0854990421e80d04c39c7c2a15a
Author: Arnaldo Carvalho de Melo <acme@conectiva.com.br>
Date:   Tue Oct 7 17:54:04 2003 -0300
    o kernel/ksyms.c: move remaining EXPORT_SYMBOLs, remove this file from the tree
Removing ksyms.c
## EXPORT_SYMBOL
History: commit f1bbcc4197c4597636782a34ad138a51c40b1efb (tag: 2.1.18)
Author: Linus Torvalds <torvalds@linuxfoundation.org>
Date:   Fri Nov 23 15:12:47 2007 -0500
    Import 2.1.18
+/* Export a symbol either from the kernel or a module.
+   In the kernel, the symbol is added to the kernel's global symbol table.
+   In a module, it controls which variables are exported.  If no
+   variables are explicitly exported, the action is controled by the 
+   insmod -[xX] flags.  Otherwise, only the variables listed are exported.
+   This obviates the need for the old register_symtab() function.  */  
## In-kernel module loader
tglx: commit aa65be3f456be1ac4359f7fffeb7b4ae3c149419
Author: Rusty Russell <rusty@rustcorp.com.au>
Date:   Sun Nov 10 17:08:07 2002 -0800
    [PATCH] In-kernel Module Loader
+/* Allocate and load the module */
+static struct module *load_module(void *umod,

# Find drivers
How to find the driver (module) associated with a device on Linux?:  https://unix.stackexchange.com/questions/97676/how-to-find-the-driver-module-associated-with-a-device-on-linux

# udev
uevent_net_init
NETLINK_KOBJECT_UEVENT
kobject_uevent and broadcast

udevadm: setup_monitor  device_monitor_new_full

systemd-udevd.service: event_queue_insert, worker_main, sd_device_monitor_start, device_monitor_event_handler, device_monitor_receive_device
#0  0x00007f48ff268680 in device_monitor_event_handler () from target:/usr/lib/systemd/libsystemd-shared-246.so
#1  0x00007f48ff28d7b7 in source_dispatch () from target:/usr/lib/systemd/libsystemd-shared-246.so
#2  0x00007f48ff28da9d in sd_event_dispatch () from target:/usr/lib/systemd/libsystemd-shared-246.so
#3  0x00007f48ff28ed68 in sd_event_run () from target:/usr/lib/systemd/libsystemd-shared-246.so
#4  0x00007f48ff28ef8f in sd_event_loop () from target:/usr/lib/systemd/libsystemd-shared-246.so
#5  0x000055eb2a150ff9 in main ()
core: worker_process_device event_free
core2: worker_device_monitor_handler can be called via sd_device_monitor_start and device_monitor_event_handler in worker_main
(gdb) bt
#0  worker_process_device (dev=0x559b0db11fd0, manager=0x559b0d9fab00) at ../src/udev/udevd.c:447
#1  worker_device_monitor_handler (monitor=0x559b0db12a20, dev=0x559b0db11fd0, userdata=0x559b0d9fab00) at ../src/udev/udevd.c:500
#2  0x0000559b0d5f03a6 in worker_main (first_device=0x559b0db11fd0, monitor=0x559b0db12a20, _manager=0x559b0d9fab00) at ../src/udev/udevd.c:562
#3  worker_spawn (event=<optimized out>, manager=0x559b0d9fab00) at ../src/udev/udevd.c:598
#4  event_run (event=<optimized out>, manager=0x559b0d9fab00) at ../src/udev/udevd.c:661
#5  event_queue_start (manager=<optimized out>) at ../src/udev/udevd.c:943
#6  0x0000559b0d5f0fc0 in on_uevent (monitor=<optimized out>, dev=0x559b0db11fd0, userdata=0x559b0d9fab00) at ../src/udev/udevd.c:1037
#7  0x00007f9ad18b06d9 in device_monitor_event_handler (s=<optimized out>, fd=<optimized out>, revents=<optimized out>, userdata=0x559b0d9fac60)
    at ../src/libsystemd/sd-device/device-monitor.c:234
#8  0x00007f9ad18d57b7 in source_dispatch (s=s@entry=0x559b0d9fc2c0) at ../src/libsystemd/sd-event/sd-event.c:3181
#9  0x00007f9ad18d5a9d in sd_event_dispatch (e=0x559b0da04b20) at ../src/libsystemd/sd-event/sd-event.c:3620
#10 0x00007f9ad18d6d68 in sd_event_run (e=e@entry=0x559b0da04b20, timeout=timeout@entry=18446744073709551615)
    at ../src/libsystemd/sd-event/sd-event.c:3678
#11 0x00007f9ad18d6f8f in sd_event_loop (e=0x559b0da04b20) at ../src/libsystemd/sd-event/sd-event.c:3700
#12 0x0000559b0d5e8ff9 in main_loop (manager=<optimized out>) at ../src/udev/udevd.c:1805
#13 run_udevd (argv=<optimized out>, argc=<optimized out>) at ../src/udev/udevd.c:1919
#14 run (argv=<optimized out>, argc=<optimized out>) at ../src/udev/udevadm.c:115
#15 main (argc=<optimized out>, argv=<optimized out>) at ../src/udev/udevadm.c:134



#0  event_free (event=0x559b0db1f820) at ../src/udev/udevd.c:152
#1  0x0000559b0d5f0c94 in on_worker (s=<optimized out>, revents=<optimized out>, userdata=<optimized out>, fd=<optimized out>)
    at ../src/udev/udevd.c:1013
#2  on_worker (s=<optimized out>, fd=6, revents=<optimized out>, userdata=0x559b0d9fab00) at ../src/udev/udevd.c:958
#3  0x00007f9ad18d57b7 in source_dispatch (s=s@entry=0x559b0d9fc3d0) at ../src/libsystemd/sd-event/sd-event.c:3181
#4  0x00007f9ad18d5a9d in sd_event_dispatch (e=0x559b0da04b20) at ../src/libsystemd/sd-event/sd-event.c:3620
#5  0x00007f9ad18d6d68 in sd_event_run (e=e@entry=0x559b0da04b20, timeout=timeout@entry=18446744073709551615)
    at ../src/libsystemd/sd-event/sd-event.c:3678
#6  0x00007f9ad18d6f8f in sd_event_loop (e=0x559b0da04b20) at ../src/libsystemd/sd-event/sd-event.c:3700
#7  0x0000559b0d5e8ff9 in main_loop (manager=<optimized out>) at ../src/udev/udevd.c:1805
#8  run_udevd (argv=<optimized out>, argc=<optimized out>) at ../src/udev/udevd.c:1919
#9  run (argv=<optimized out>, argc=<optimized out>) at ../src/udev/udevadm.c:115
#10 main (argc=<optimized out>, argv=<optimized out>) at ../src/udev/udevadm.c:134

sd_event_add_io(manager->event, NULL, fd_worker, EPOLLIN, on_worker, manager);

(gdb) bt
#0  event_free (event=0x0) at ../src/udev/udevd.c:152
#1  0x0000559b0d5ed000 in worker_free (worker=0x559b0db1cae0) at ../src/udev/udevd.c:184
#2  0x0000559b0d5f1aa8 in on_sigchld (s=<optimized out>, si=<optimized out>, userdata=<optimized out>) at ../src/udev/udevd.c:1386
#3  on_sigchld (s=<optimized out>, si=<optimized out>, userdata=0x559b0d9fab00) at ../src/udev/udevd.c:1334
#4  0x00007f9ad18d5702 in source_dispatch (s=s@entry=0x559b0da04420) at ../src/libsystemd/sd-event/sd-event.c:3193
#5  0x00007f9ad18d5a9d in sd_event_dispatch (e=0x559b0da04b20) at ../src/libsystemd/sd-event/sd-event.c:3620
#6  0x00007f9ad18d6d68 in sd_event_run (e=e@entry=0x559b0da04b20, timeout=timeout@entry=18446744073709551615)
    at ../src/libsystemd/sd-event/sd-event.c:3678
#7  0x00007f9ad18d6f8f in sd_event_loop (e=0x559b0da04b20) at ../src/libsystemd/sd-event/sd-event.c:3700
#8  0x0000559b0d5e8ff9 in main_loop (manager=<optimized out>) at ../src/udev/udevd.c:1805
#9  run_udevd (argv=<optimized out>, argc=<optimized out>) at ../src/udev/udevd.c:1919
#10 run (argv=<optimized out>, argc=<optimized out>) at ../src/udev/udevadm.c:115
#11 main (argc=<optimized out>, argv=<optimized out>) at ../src/udev/udevadm.c:134

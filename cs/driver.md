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

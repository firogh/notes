---
tags: [ kernel ] 
title: The definitive guide to Linux x86 entry.S
date: 2017-04-26T21:39:41+08:00 
category: cs
---

Documentation/x86/entry_64.txt
[对Linux x86-64架构上硬中断的重新认识](http://www.lenky.info/archives/2013/03/2245)



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

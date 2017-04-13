---
tags: [ cs ] 
title: Hello world! a journey to computer science
date: 2017-04-13T20:42:49+08:00 
category: cs
---

In this series, I will a simple hello world program to explain the every important concept in cmputer science.
So let's go.


Do you know vDSO?
If a C program, gtod, calls the system call gettimeofday in vDSO, how does this program know the address of gettimeofday?

ldd gtod 
	linux-gate.so.1 (0xf7766000)
	libdl.so.2 => /lib/libdl.so.2 (0xf7743000)
	libc.so.6 => /lib/libc.so.6 (0xf7568000)
	/lib/ld-linux.so.2 (0x56639000)
Check this commit of glibc:
30e32d23eb06cbeae02e51c7c4df150736dba795
Set up GLRO(dl_sysinfo_dso) under [NEED_DL_SYSINFO_DSO] as well
AT_SYSINFO_EHDR


# start the program
sysdeps/x86_64/dl-machine.h: -> _start ->_dl_start->/elf/dl-sysdep.c:83:_dl_sysdep_start->dl_main

# Reference
[Linkers 20 parts by Gold author Ian Lance Taylor](http://a3f.at/lists/linkers)
[The Definitive Guide to Linux System Calls](https://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls/#locating-the-vdso-in-memory)

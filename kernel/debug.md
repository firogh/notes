---
layout: post
date: 2015-01-03
title: Kernel debug method
category: kernel
---

#Debugging support in the kernel

#Debugging by Printing
Marco
ratelimit
console level
Print by qurey:proc ioctl

#Debugging by Watching
strace

#Debug tools and 
gdb /usr/src/linux/vmlinux /proc/kcore
objdump -s vmlinux
kdb
./scripts/decodecode < Oops.txt

#Skill
=Generating signl assembly source file!
make kernel/sched.s V=1

#If an page oops close to zero, for example 0xfffffff4
It maybe ERR_PTR(-12);

1. 踩内存

2. recursive stack,PC_MEMLOCK 

3. deadlock, __GFP_FS

4. NULL poniter reference, functions adn varibles

#ASM
move <-
j long jump
b short jump
a0, a1...99% is parameters.





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


#From ASM to c language
* expand inline function
* locate __asm__() 内嵌汇编, 能快速定位代码! 但很少! slhc_uncompress()
* 找常量!
* loop codes formate!
+ef8:   00a01021    move    v0,a1
 efc:   88440003    lwl a0,3(v0)
 f00:   24450004    addiu   a1,v0,4                                                                                                           
 f04:   98440000    lwr a0,0(v0)                                                                                                               
 f08:   00641821    addu    v1,v1,a0                                                                                                            
 f0c:   0064202b    sltu    a0,v1,a0                                                                                                             
+f10:   14a7fff9    bne a1,a3,ef8 <slhc_uncompress+0x444>                                                                                         
 f14:   00831821    addu    v1,a0,v1



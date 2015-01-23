---
layout: post
date: 2014-12-28
title: Memory barrier
category: kernel
---

#Refernce
http://en.wikipedia.org/wiki/Memory_barrier
Documentation/memory-barriers.txt

#Common
* When a program runs on a single-CPU machine, the hardware performs the necessary bookkeeping 
to ensure that the program executes as if all memory operations were performed in the order 
specified by the programmer (program order), so memory barriers are not necessary. 
However, when the memory is shared with multiple devices, such as other CPUs in a multiprocessor 
system, or memory mapped peripherals, out-of-order access may affect program behavior. 
For example, a second CPU may see memory changes made by the first CPU in a sequence which differs from program order.
* Compiler and cpu do the same optimization: reorder of instructions

#The Linux kernel has a variety of different barriers that act at different
levels:
  (*) Compiler barrier.
  (*) CPU memory barriers.
  (*) MMIO write barrier.

#ACCESS_ONCE
Does it work cast a variable to volatile?
http://stackoverflow.com/questions/22758232/why-is-access-once-so-complex

#For volatile
http://www.geeksforgeeks.org/understanding-volatile-qualifier-in-c/

---
tags: [ cs ] 
title: Memory-ordering model
date: 2017-12-16T15:46:12+08:00 
category: cs
---

When we are talking on memory model, we are refering memory consistency model or memory ordering model.

# ETC
[Why do we need mb for SLEEP AND WAKE-UP FUNCTIONS?](https://www.kernel.org/doc/Documentation/memory-barriers.txt)

# Practices
[Memory Reordering Caught in the Act](http://preshing.com/20120515/memory-reordering-caught-in-the-act/)

# Memory model
[Shared Memory Consistency Models: A Tutorial](http://www.hpl.hp.com/techreports/Compaq-DEC/WRL-95-7.pdf)
[Memory Consistency Models: A Primer](https://homes.cs.washington.edu/~bornholt/post/memory-models.html#fn:lamport)
[Weak vs. Strong Memory Models](http://preshing.com/20120930/weak-vs-strong-memory-models/)

# x86 memory model
[Memory Barriers: a Hardware View for Software Hackers - must read](http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf)
[x86 is TSO why it borthers fence?](https://bartoszmilewski.com/2008/11/05/who-ordered-memory-fences-on-an-x86/)
[共同进入与饥饿](https://www.cnblogs.com/caidi/p/6708789.html)
[When are x86 LFENCE, SFENCE and MFENCE instructions required?](https://stackoverflow.com/questions/27595595/when-are-x86-lfence-sfence-and-mfence-instructions-required)

# C11 memory model
[C memory order](http://en.cppreference.com/w/c/atomic/memory_order)
[Outlawing Ghosts: Avoiding Out-of-Thin-Air Results](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/42967.pdf)
[Out-of-Thin-Air Execution is Vacuous](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4375.html)
[Relaxed-Memory Concurrency](http://www.cl.cam.ac.uk/~pes20/weakmemory/)
[Mathematizing C++ Concurrency: The Isabelle/HOL model](https://www.cl.cam.ac.uk/~pes20/cpp/model.pdf)

# C++ memory model
[The C++ Memory Model - Valentin Ziegler @ Meeting C++ 2014](https://www.youtube.com/watch?v=gpsz8sc6mNU)

# Compiler memory model
[Memory Ordering at Compile Time](http://preshing.com/20120625/memory-ordering-at-compile-time/)
[A Guide to Undefined Behavior in C and C++, Part 3](https://blog.regehr.org/archives/232)

# Linux kernel memory model
[A formal kernel memory-ordering model](https://lwn.net/Articles/718628/)
[A formal kernel memory-ordering model (part 2)](https://lwn.net/Articles/720550/)
[Linux-Kernel Memory Model](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4374.html)
[Linux-Kernel Memory Model](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0124r4.html)
[Linux-Kernel Memory Model](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0124r6.html)
[Linux-Kernel Memory Ordering: Help Arrives At Last!](http://events.linuxfoundation.org/sites/events/files/slides/LinuxMM.2016.09.19a.LCE_.pdf) and [Talk on youtube on this!](https://www.youtube.com/watch?v=ULFytshTvIY)

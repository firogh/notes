---
tags: [ cs ] 
title: Memory-ordering model
date: 2017-12-16T15:46:12+08:00 
category: cs
---

When we are talking on memory model, we are refering memory consistency model or memory ordering model.

[0]: http://www.hpl.hp.com/techreports/Compaq-DEC/WRL-95-7.pdf
[1]: http://preshing.com/20120930/weak-vs-strong-memory-models/
[4]: https://bartoszmilewski.com/2008/11/05/who-ordered-memory-fences-on-an-x86/
[5]: http://preshing.com/20120515/memory-reordering-caught-in-the-act/
[6]: http://en.cppreference.com/w/c/atomic/memory_order
[7]: https://stackoverflow.com/questions/27595595/when-are-x86-lfence-sfence-and-mfence-instructions-required
[8]: http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf
[9]: http://preshing.com/20120625/memory-ordering-at-compile-time/
[10]: https://blog.regehr.org/archives/232

[11]: http://www.cl.cam.ac.uk/~pes20/weakmemory/
[12]: https://lwn.net/Articles/718628/
[13]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4374.html
[14]: http://events.linuxfoundation.org/sites/events/files/slides/LinuxMM.2016.09.19a.LCE_.pdf
[15]: https://www.youtube.com/watch?v=ULFytshTvIY
[16]: https://lwn.net/Articles/720550/
[17]: https://www.cl.cam.ac.uk/~pes20/cpp/model.pdf
[18]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4375.html
[19]: https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/42967.pdf
[20]: https://homes.cs.washington.edu/~bornholt/post/memory-models.html#fn:lamport

# Practices
[Memory Reordering Caught in the Act][5]

# Memory model
[Memory Consistency Models: A Primer][20]
[Shared Memory Consistency Models: A Tutorial][0]
[Weak vs. Strong Memory Models][1]

# x86 memory model
[Memory Barriers: a Hardware View for Software Hackers - must read][8]
[x86 is TSO why it borthers fence?][4]
[共同进入与饥饿](https://www.cnblogs.com/caidi/p/6708789.html)
[When are x86 LFENCE, SFENCE and MFENCE instructions required?][7]

# C11 memory model
[C memory order][6]
[Outlawing Ghosts: Avoiding Out-of-Thin-Air Results][19]
[Out-of-Thin-Air Execution is Vacuous][18]
[Relaxed-Memory Concurrency][11]
[Mathematizing C++ Concurrency: The Isabelle/HOL model][17]

# Compiler memory model
[Memory Ordering at Compile Time][9]
[A Guide to Undefined Behavior in C and C++, Part 3][10]

# Linux kernel memory model
[A formal kernel memory-ordering model][12]
[A formal kernel memory-ordering model (part 2)][16]
[Linux-Kernel Memory Model][13]
[Linux-Kernel Memory Ordering: Help Arrives At Last!][14] and [Talk on youtube on this!][15]


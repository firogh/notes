---
tags: [ cs ] 
title: Memory consistency model
date: 2017-12-16T15:46:12+08:00 
category: cs
---

When we are talking on memory model, we are refering memory consistency model or memory ordering model.

# Hisotry
1979
[How to Make a Multiprocessor Computer That Correctly Executes Multiprocess Progranm](https://www.microsoft.com/en-us/research/uploads/prod/2016/12/How-to-Make-a-Multiprocessor-Computer-That-Correctly-Executes-Multiprocess-Programs.pdf)
1987 ~ 1990 
[Linearizability: A Correctness Condition for Concurrent Objects](https://cs.brown.edu/~mph/HerlihyW90/p463-herlihy.pdf)
1989
[processor consistency: CACHE CONSISTENCY AND SEQUENTIAL CONSISTENCY](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.8.3766&rep=rep1&type=pdf)
1990
[Release consistency: Memory consistency and event ordering in scalable shared-memory multiprocessors](https://dl.acm.org/citation.cfm?id=325102)
1991
[Proving sequential consistency of high-performance shared memories](https://dl.acm.org/citation.cfm?id=113406)
1992
[TSO Sparc v8: A standard memory model called Total Store Ordering (TSO) is defined for SPARC](https://www.gaisler.com/doc/sparcv8.pdf)
[Formal Specification of Memory Models: and two store ordered models TSO and PSO defined by the Sun Microsystem's SPARC architecture.](https://link.springer.com/chapter/10.1007/978-1-4615-3604-8_2)

2001 ~ Present
[IA64 memory ordering](https://www.youtube.com/watch?v=WUfvvFD5tAA)

# Purposes
[Motivation: hiding latency](https://www.cs.cmu.edu/afs/cs/academic/class/15418-s12/www/lectures/14_relaxedReview.pdf)
▪ Why are we interested in relaxing ordering requirements?
- Performance
- Speci!cally, hiding memory latency: overlap memory accesses with other operations
- Remember, memory access in a cache coherent system may entail much more then
simply reading bits from memory (!nding data, sending invalidations, etc.)
## Why TSO? [It's because that write buffer or Store buffer is not invisible any more for multiprocessor](https://www.cis.upenn.edu/~devietti/classes/cis601-spring2016/sc_tso.pdf)
To abandon SC; to Allow use of a FIFO write buffer.
[An example: There’s no reason why performing event (2) (a read from B) needs to wait until event (1) (a write to A) completes. They don’t interfere with each other at all, and so should be allowed to run in parallel. See Memory Consistency Models: A Primer](https://www.cs.utexas.edu/~bornholt/post/memory-models.html)
Hide the write latency by putting the data in the store buffer.
### Why not read-write reordering?
reordering read-write is non-sense.

# Formal cause
Shared memory
Multiprocessor
Memory access
program order
[Recommened by CAAQA: Observity in SC, TSO, PC: Paragraph Relaxing the Write to Read Program Order in Shared Memory Consistency Models: A Tutorial](https://www.hpl.hp.com/techreports/Compaq-DEC/WRL-95-7.pdf) 
[Memory Barriers: a Hardware View for Software Hackers - must read](http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf)
['A Summary of Relaxed Consistency' CMU](http://15418.courses.cs.cmu.edu/spring2013/article/41)[Slides](https://www.cs.cmu.edu/afs/cs/academic/class/15418-s12/www/lectures/14_relaxedReview.pdf)
## SC
[sequential consistency](https://www.microsoft.com/en-us/research/uploads/prod/2016/12/How-to-Make-a-Multiprocessor-Computer-That-Correctly-Executes-Multiprocess-Programs.pdf)
[Formal of Sequential Consistency by Jepsen](https://jepsen.io/consistency/models/sequential#formally)

## TSO 
Total Store Ordering in Appendix k Sparc v8.
### TSO in x86
[A Better x86 Memory Model: x86-TSO](https://www.cl.cam.ac.uk/~pes20/weakmemory/x86tso-paper.tphols.pdf)
[When are x86 LFENCE, SFENCE and MFENCE instructions required?](https://stackoverflow.com/questions/27595595/when-are-x86-lfence-sfence-and-mfence-instructions-required)
### TSO vs PC: 
['A Summary of Relaxed Consistency' CMU](http://15418.courses.cs.cmu.edu/spring2013/article/41)[Slides](https://www.cs.cmu.edu/afs/cs/academic/class/15418-s12/www/lectures/14_relaxedReview.pdf)
### TSO and Peterson's algorithm
[Who ordered memory fences on an x86?](https://bartoszmilewski.com/2008/11/05/who-ordered-memory-fences-on-an-x86/)
[共同进入与饥饿](https://www.cnblogs.com/caidi/p/6708789.html)

## PC
[processor consistency: CACHE CONSISTENCY AND SEQUENTIAL CONSISTENCY](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.8.3766&rep=rep1&type=pdf)

## WC
[weak consistency: Memory access buffering in multiprocessors](https://people.eecs.berkeley.edu/~kubitron/cs252/handouts/oldquiz/p434-dubois.pdf)
They distinguish between ordinary shared accesses and synchronization accesses, where the latter are used to control concurrency
between several processes and to maintain the integrity of ordinary shared data.

## RC
[Firo: a must-read: Release consistency: Memory consistency and event ordering in scalable shared-memory multiprocessors](https://dl.acm.org/citation.cfm?id=325102)
[Acquire and Release Semantics](https://preshing.com/20120913/acquire-and-release-semantics/)

## Kernel
[Why do we need mb for SLEEP AND WAKE-UP FUNCTIONS?](https://www.kernel.org/doc/Documentation/memory-barriers.txt)
[A formal kernel memory-ordering model](https://lwn.net/Articles/718628/)
[A formal kernel memory-ordering model (part 2)](https://lwn.net/Articles/720550/)
[Linux-Kernel Memory Model](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4374.html)
[Linux-Kernel Memory Model](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0124r4.html)
[Linux-Kernel Memory Model](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0124r6.html)
[Linux-Kernel Memory Ordering: Help Arrives At Last!](http://events.linuxfoundation.org/sites/events/files/slides/LinuxMM.2016.09.19a.LCE_.pdf) and [Talk on youtube on this!](https://www.youtube.com/watch?v=ULFytshTvIY)
## Compiler
[Memory Ordering at Compile Time](http://preshing.com/20120625/memory-ordering-at-compile-time/)
[A Guide to Undefined Behavior in C and C++, Part 3](https://blog.regehr.org/archives/232)

## Other architectures
[Relaxed-Memory Concurrency](http://www.cl.cam.ac.uk/~pes20/weakmemory/)

## C11(library) memory model
[C memory order](http://en.cppreference.com/w/c/atomic/memory_order)
[Don't read: The Thin-air Problem](https://www.cl.cam.ac.uk/~pes20/cpp/notes42.html)
[Outlawing Ghosts: Avoiding Out-of-Thin-Air Results](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/42967.pdf)
[Out-of-Thin-Air Execution is Vacuous](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4375.html)

# Material
## Practices
[Memory Reordering Caught in the Act](http://preshing.com/20120515/memory-reordering-caught-in-the-act/)

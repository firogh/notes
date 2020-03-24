---
tags: [ cs ] 
title: Firo's perspective on computer science
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Computation
Program: programming, Concurrency
I/O: Locality of reference, memory hierarchy
Calculation: Parallelism, ILP, superscalar, SMP
Memory storage: memory, memory management, VM(layered smantic), the locality principle

# Memory management
Allocation: buddy track system, metric, page allocator. PFRA & workingset, rmap
VM: address space, page tables, tlb, page fault

# VM
Decopule addresses and memory locations

# Paging
Lift the burden of loading program from application

# Program
Completion time

# Concurrency
[EW Dijkstra: Cooperating sequential processes](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD01xx/EWD123.html)
## concurrent vs parallel
Programming vs computing: software vs hardware
concurrent programming: multi-thread stress on how to create thread to model the outside world.
parallel programming: smp, synchronization, lock, lock-free, atomic
[Linus: The Whole "Parallel Computing Is The Future" Is A Bunch Of Crock.](http://highscalability.com/blog/2014/12/31/linus-the-whole-parallel-computing-is-the-future-is-a-bunch.html)
[Linus: Avoiding ping pong](https://www.realworldtech.com/forum/?threadid=146066&curpostid=146227)
## Concurrency problem 1 with communications
IPC check wait.log
## Problem 2 with memory access
consistency model

# Metric
Bandwidth and latency
Scalability
Performance
Easy to use
Security/Protection/isolation
Reliability
Energy-efficiency

# History
[Atlas](http://www.chilton-computing.org.uk/acl/technology/atlas/overview.htm)

# OS
[Operating Systems: Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/)
## Unix
[Ken Thompson UNIX Implementation](https://users.soe.ucsc.edu/~sbrandt/221/Papers/History/thompson-bstj78.pdf)
[The Evolution of the UNIX Time-sharing System](https://www.cs.grinnell.edu/~curtsinger/teaching/2019S/CSC213/files/unix_evolution.pdf)
[Twenty Years of Berkeley Unix From AT&T-Owned to Freely Redistributable](https://www.oreilly.com/openbook/opensources/book/kirkmck.html)
[The UNIX TimeSharing System Dennis M. Ritchie and Ken Thompson Bell Laboratories](https://people.eecs.berkeley.edu/~brewer/cs262/unix.pdf)
[The UNIX Time-sharing SystemA Retrospective](https://www.bell-labs.com/usr/dmr/www/retro.pdf)
[SunOS 1.0 - 4.1.2](http://bitsavers.trailing-edge.com/pdf/sun/sunos/)
Life with Unix
[Design and Implementation of the Berkeley Virtual Memory Extensions to the UNIX† Operating System‡](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.107.9119&rep=rep1&type=pdf)

# Specifications
[Linux Foundation Referenced Specifications](http://refspecs.linuxbase.org/)

# Computer science
[Core topics of computer science](https://computersciencewiki.org/index.php/Welcome)
Underlying our approach to this subject is our conviction that “computer science” is not a science and that its significance has little to do with computers. The computer revolution is a revolution in the way we think and in the way we express what we think. The essence of this change is the emergence of what might best be called procedural epistemology —the study of the structure of knowledge from an imperative point of view, as opposed to the more declarative point of view taken by classical mathematical subjects. Mathematics provides a framework for dealing precisely with notions of “what is.” Computation provides a framework for dealing precisely with notions of “how to.”  -- The omnipotent SICP
[Richard Feynman on Computer Science — Talk at Bell Labs (1985)](https://www.youtube.com/watch?v=lL4wg6ZAFIM)
[The Philosophy of Computer Science](https://plato.stanford.edu/entries/computer-science/)

# TODO
## Assembly programming
[Vijay Kumar B's Embedded Programming with the GNU Toolchain](http://www.bravegnu.org/gnu-eprog/)
[Bootloader with qemu](https://stackoverflow.com/questions/38041478/int-10h-not-working-in-qemu)

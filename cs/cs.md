--
tags: [ cs ] 
title: Firo's perspective on computer science
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Perspectives
Causality model: Turing machine
Resource
Application: complexcity, CS Hardware(Turing-completeness) and software(programming)
work
Workload
Optimization
Observability

# Resources
Allocation: compressible
Use: complexcity
Unit
Reclaim:
http://concurrencyfreaks.blogspot.com/2017/08/why-is-memory-reclamation-so-important.html
“The Landscape and Exciting New Future of Safe Reclamation for High Performance” 27:00
## Resource analysis
[compressible vs incompressible resource](https://en.wikipedia.org/wiki/System_resource)
[The Kubernetes resource model](https://github.com/kubernetes/kubernetes/blob/release-1.1/docs/design/resources.md)
Time-based vs space-based resource.

# Causality model
Causality, resource, use
Turing machine, turing completeness. Symbols, states, instructions, read-write-move head, tape.
Computations: a series of data and branch operation, arithmetic and logic operations and IO operations.
Processor, storage.

# Computer
[Build an 8-bit computer from scratch](https://eater.net/8bit)
Processor
Interrupt and exception
Storage: Memory hierarchy, FS, VFS
Memory: VM, the locality principle
IO: Caching, Locality of reference, memory hierarchy
Security, protection ring

# IO
User interface
Storage allocation
Storage location
Memory area for IO data
Queue
Driver
Hardware
Completion

# Speed, Work, computing, execution time
[Time, space efficiency](https://en.wikipedia.org/wiki/Algorithmic_efficiency)
worker, worker speed, worker stall, work stall, work response.
Frequency, latency, response time.
Execution time.
Parallelism Processor
https://en.wikipedia.org/wiki/Program_optimization

# Latency, locality, cache

# cache, dirty, data loss, writeback
dirty paths
dirty ratio: dirty pages / all pages in page cache

# data race, processor clock, synchronous, completion, predictable, why synchronous important?
Clock, lock, synchronous circuits,与clock一起.
Clock: [why does processor need a clock?](https://qr.ae/pNyDsD)
race, propergation delay, j-k flip-flop, synchronize
https://www.youtube.com/watch?v=st3mUEub99E
predicatable
https://www.quora.com/Can-you-explain-in-details-why-we-need-a-processor-clock-what-it-does-what-happens-if-we-remove-it
synchronous circuit: with clock-> wait for completing -> synchronous IO: wait for I/O completation.
synchronize: coordinate
## Synchronized, Synchronous, and Asynchronous Operations
Check LSP 3rd Chapter 4.
Synchronous I/O is a user space notation.
Synchronous here actually means wait or blocking.
Execution of process waits for I/O operations. I/O operations can be explained in different level e.g. system call, buffer cache.
[As Discrete lizard suggests, there is not some precise CS-wide definition for these terms. ... Nevertheless, a vague general statement is that in "synchronous" systems, things "wait" for other things, while in "asynchronous" systems, things don't "wait".](https://cs.stackexchange.com/questions/87195/what-does-synchronous-and-asynchronous-mean-in-computer-science)
[depend on previous task](https://stackoverflow.com/a/7132111/1025001)

## https://www.yourdictionary.com/synchronous
From Latin and Greek origins, synchronous translates as together with time. Referring to events that occur at the same instant of a coordinated time scale. If the events are repetitive, the instant of one event bears a fixed time relationship with the instant of a corresponding event, e.g., event a is followed 10 milliseconds later by event b. Synchronous processes in separate, networked devices depend on a common clocking source, on clocking pulses emitted by the transmitting device, or on synchronizing bits or bit patterns embedded in a set of data.

## https://www.quora.com/Are-synchronous-and-asynchronous-transmission-different-types-of-parallel-transmission
Phillip Remaker, connecting your computers since 1988
Synchronous: (etymology: sun (together) khronos (time))
In a synchronous system, there is a continuous time pulse signal (clock) that runs together with the data signal to indicate when sample should be taken. Serial or parallel does not matter; it only matters that a dedicated clock signal tells the receiver when to sample the data.
In an asynchronous system (without together time), the sampling clock is embedded in and extracted from the data, and the data transmission can start and stop at will.

# synchronous, asynchronous, completion
cause, effect; begin and end
limited states, order, exception, error
next state
## Mark of end 
Variable: completion
Callback:

# Lock
lock-free: https://www.youtube.com/watch?v=mHwU0TMuwgc

# Program, OS
http://www.joelfernandes.org/
TIF_NEED_RESCHED: why is it needed http://www.joelfernandes.org/linuxinternals/2016/03/20/tif-need-resched-why-is-it-needed.html
Concurrency and stack, event, task, user vs. system, system calls


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
## Linux 
[Linux Foundation Referenced Specifications](http://refspecs.linuxbase.org/)

# Programming
## Function/[subroutine](https://en.wikipedia.org/wiki/Subroutine) and 
1945 Turing on subroutines in [Proposed electronic calculator.](http://www.alanturing.net/turing_archive/archive/p/p01/P01-011.html): In Chapter 6. Outline of Logical Control. We also wish to be able to arrange for the splitting up of operations into... When we wish to start on a subsidiary operation we need only make a note of where we left off the major operation...
## stack
[Firo-must: Dijkstra, E. W. (1960). "Recursive Programming"](https://link.springer.com/article/10.1007%2FBF01386232)
[call stack](https://en.wikipedia.org/wiki/Call_stack)
Stack overflow: gurad page
Stack based buffer overflow: canary, STACKPROTECTOR, Stack Protector buffer overflow detection
Related code: boot_init_stack_cana

# Concurrency
[EW Dijkstra: Cooperating sequential processes](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD01xx/EWD123.html)
https://en.wikipedia.org/wiki/Concurrent_computing
https://slikts.github.io/concurrency-glossary/
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
## Scalability - Lockless concurrency
[What every systems programmer should know about lockless concurrency](https://news.ycombinator.com/item?id=15607869)

# Memory allocation
buddy track system, metric, page allocator. PFRA & workingset, rmap

# VM
Lift the burden of loading program from application.
address space, page tables, tlb, page fault, paging
Decopule addresses and memory locations

# History
[Atlas](http://www.chilton-computing.org.uk/acl/technology/atlas/overview.htm)

# Computer science
[Core topics of computer science](https://computersciencewiki.org/index.php/Welcome)
Underlying our approach to this subject is our conviction that “computer science” is not a science and that its significance has little to do with computers. The computer revolution is a revolution in the way we think and in the way we express what we think. The essence of this change is the emergence of what might best be called procedural epistemology —the study of the structure of knowledge from an imperative point of view, as opposed to the more declarative point of view taken by classical mathematical subjects. Mathematics provides a framework for dealing precisely with notions of “what is.” Computation provides a framework for dealing precisely with notions of “how to.”  -- The omnipotent SICP
[Richard Feynman on Computer Science — Talk at Bell Labs (1985)](https://www.youtube.com/watch?v=lL4wg6ZAFIM)
[The Philosophy of Computer Science](https://plato.stanford.edu/entries/computer-science/)

# TODO
## Assembly programming
[Vijay Kumar B's Embedded Programming with the GNU Toolchain](http://www.bravegnu.org/gnu-eprog/)
[Bootloader with qemu](https://stackoverflow.com/questions/38041478/int-10h-not-working-in-qemu)

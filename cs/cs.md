---
tags: [ cs ] 
title: Computing Sorcery a sorcerer's perspective on computer science
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Computer science
[Firo: very good CS website: Computational thinking & problem-solving](https://computersciencewiki.org/index.php/Computational_thinking_%26_problem-solving)
[Core topics of computer science](https://computersciencewiki.org/index.php/Welcome)
Underlying our approach to this subject is our conviction that “computer science” is not a science and that its significance has little to do with computers. The computer revolution is a revolution in the way we think and in the way we express what we think. The essence of this change is the emergence of what might best be called procedural epistemology —the study of the structure of knowledge from an imperative point of view, as opposed to the more declarative point of view taken by classical mathematical subjects. Mathematics provides a framework for dealing precisely with notions of “what is.” Computation provides a framework for dealing precisely with notions of “how to.”  -- The omnipotent SICP
[Richard Feynman on Computer Science — Talk at Bell Labs (1985)](https://www.youtube.com/watch?v=lL4wg6ZAFIM)
[The Philosophy of Computer Science](https://plato.stanford.edu/entries/computer-science/)
## Firo
computation, storage, IO
Architecture: Memory hierarchy
OS: VM(layered smantic), Concurrency
Application: 

# Tools
Parallemism and locality
Bandwidth and latency

# Problems
Complexity
Resource management
Observation
Latency
Scalibity
Performance/efficiency
Easy to use
Security/Protection/isolation
Reliability
Energy-efficiency

# Computation
## Function/[subroutine](https://en.wikipedia.org/wiki/Subroutine) and [call stack](https://en.wikipedia.org/wiki/Call_stack)
1945 Turing on subroutines in [Proposed electronic calculator.](http://www.alanturing.net/turing_archive/archive/p/p01/P01-011.html): In Chapter 6. Outline of Logical Control. We also wish to be able to arrange for the splitting up of operations into... When we wish to start on a subsidiary operation we need only make a note of where we left off the major operation...
[Firo-must: Dijkstra, E. W. (1960). "Recursive Programming"](https://link.springer.com/article/10.1007%2FBF01386232)
## stack
Stack overflow: gurad page
Stack based buffer overflow: canary, STACKPROTECTOR, Stack Protector buffer overflow detection

Related code: boot_init_stack_canary

## process vs event
[thread vs event](http://courses.cs.vt.edu/cs5204/fall09-kafura/Presentations/Threads-VS-Events.pdf)

# Concurrency
[Firo: must thinking summary: EW Dijkstra: Cooperating sequential processes](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD01xx/EWD123.html)
CSAPP3e: Chapter 12 concurrent programming
Parallel and Concurrent Programming in Haskell
The origin of concurrent programming: from semaphores to remote procedure calls - Per Brinch Hansen
[Firo: good introduction on history of concurrency: Shared-Memory Concurrency](https://www.dcl.hpi.uni-potsdam.de/teaching/pvprog/Slides/C1_concurrency.pdf)
[Turing lecture: The computer science of concurrency: the early years](https://dl.acm.org/citation.cfm?id=2771951)
[Firo: example: Concurrency-managed workqueues](https://lwn.net/Articles/355700/)
[Rob Pike: Concurrency is the composition of independently executing computations.](https://talks.golang.org/2012/concurrency.slide#6)
## [Asynchrony (computer programming)](https://en.wikipedia.org/wiki/Asynchrony_(computer_programming))

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


# Resource management
check mm.md

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


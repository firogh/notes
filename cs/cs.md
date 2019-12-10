---
tags: [ cs ] 
title: Computing Sorcery a sorcerer's perspective on computer science
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# History
[Atlas](http://www.chilton-computing.org.uk/acl/technology/atlas/overview.htm)

# Formal causes
[Ken Thompson UNIX Implementation](https://users.soe.ucsc.edu/~sbrandt/221/Papers/History/thompson-bstj78.pdf)
[Design and Implementation of the Berkeley Virtual Memory Extensions to the UNIX† Operating System‡](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.107.9119&rep=rep1&type=pdf)
Process control: Process creation and  program execution, Swapping/paging, Synchronization, scheduling
Memory management: paged VM
I/O system; filesystem

# Problems
Asynchronous
latency
scalibity

# Concurrency
CSAPP3e: Chapter 12 concurrent programming
Parallel and Concurrent Programming in Haskell
The origin of concurrent programming: from semaphores to remote procedure calls - Per Brinch Hansen
[Cooperating sequential processes](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD01xx/EWD123.html)
[Firo: good introduction on history of concurrency: Shared-Memory Concurrency](https://www.dcl.hpi.uni-potsdam.de/teaching/pvprog/Slides/C1_concurrency.pdf)
[Turing lecture: The computer science of concurrency: the early years](https://dl.acm.org/citation.cfm?id=2771951)
[Firo: example: Concurrency-managed workqueues](https://lwn.net/Articles/355700/)
## concurrent vs parallel
Programming vs computing: software vs hardware
concurrent programming: multi-thread stress on how to create thread to model the outside world.
parallel programming: smp, synchronization, lock, lock-free, atomic


# Documents
[ Linux Foundation Referenced Specifications](http://refspecs.linuxbase.org/)

[0]: https://www.youtube.com/watch?v=lL4wg6ZAFIM
[1]: https://github.com/shadowsocks/shadowsocks/tree/master
[2]: http://www.gigamonkeys.com/code-reading/
[3]: https://plato.stanford.edu/entries/computer-science/

What's the computer science? Have you ever asked yourself? It's not a science! 
Underlying our approach to this subject is our conviction that “com-
puter science” is not a science and that its significance has little to do
with computers. The computer revolution is a revolution in the way we
think and in the way we express what we think. The essence of this
change is the emergence of what might best be called procedural epis-
temology —the study of the structure of knowledge from an imperative
point of view, as opposed to the more declarative point of view taken
by classical mathematical subjects. Mathematics provides a framework
for dealing precisely with notions of “what is.” Computation provides a
framework for dealing precisely with notions of “how to.”  -- The omnipotent SICP
[Richard Feynman on Computer Science — Talk at Bell Labs (1985)][0]
[Reading code? CODE IS NOT LITERATURE][2]
[The Philosophy of Computer Science][3]
[Mort Yao 程序与证明](http://www.tuicool.com/articles/YRZvem)
[Vijay Kumar B's Embedded Programming with the GNU Toolchain](http://www.bravegnu.org/gnu-eprog/)

performance/efficiency
Easy to use
Security/Protection/isolation
Reliability
Energy-efficiency



# The abilities of Computer science programmer
Debugging
Memory management

# Unrelatted but interesting topic on memory models
[The memory models that underlie programming languages](http://canonical.org/~kragen/memory-models/)
[History of Memory Models](https://www.youtube.com/watch?v=3e1ZF1L1VhY)

# The fundermental
Buffer: for bandwidth
Cache: for speed

# FAQ
[All about pseudo](https://www.ibm.com/developerworks/library/os-aapseudo1/os-aapseudo1-pdf.pdf)

## Procedure or subroutine
https://people.cs.clemson.edu/~mark/subroutines.html
https://en.wikipedia.org/wiki/Subroutine
* 1945 Turing on subroutines in [Proposed electronic calculator.](http://www.alanturing.net/turing_archive/archive/p/p01/P01-011.html)
In Chapter 6. Outline of Logical Control.
We also wish to be able to arrange for the splitting up of operations into...
When we wish to start on a subsidiary operation we need only make a note
of where we left off the major operation...
* 1952 [The use of sub-routines in programmes](http://www.laputan.org/pub/papers/wheeler.pdf)
The above remarks may be summarized by saying sub-routines are very useful — although not absolutely necessary — and that the prime objectives to be born     in mind when constructing them are simplicity of use, correctness of codes and accuracy of description. All complexities should—if possible—be buried out     of sight.

## Linkage
1960 [Dijkstra, E. W. (1960). "Recursive Programming"](https://link.springer.com/article/10.1007%2FBF01386232)
https://en.wikipedia.org/wiki/Link_register
?? [The Linux x86 ORC Stack Unwinder](http://www.codeblueprint.co.uk/2017/07/31/the-orc-unwinder.html)
[The ORCs are coming](https://lwn.net/Articles/728339/)

# Unix
[The Evolution of the UNIX Time-sharing System](https://www.cs.grinnell.edu/~curtsinger/teaching/2019S/CSC213/files/unix_evolution.pdf)
[Twenty Years of Berkeley Unix From AT&T-Owned to Freely Redistributable](https://www.oreilly.com/openbook/opensources/book/kirkmck.html)
[The UNIX TimeSharing System Dennis M. Ritchie and Ken Thompson Bell Laboratories](https://people.eecs.berkeley.edu/~brewer/cs262/unix.pdf)
[The UNIX Time-sharing SystemA Retrospective](https://www.bell-labs.com/usr/dmr/www/retro.pdf)
[SunOS 1.0 - 4.1.2](http://bitsavers.trailing-edge.com/pdf/sun/sunos/)
Life with Unix

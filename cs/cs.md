---
tags: [ cs ] 
title: Computing Sorcery a sorcerer's perspective on computer science
date: 2015-02-27T15:46:14+08:00 
category: cs
---

[0]: https://www.youtube.com/watch?v=lL4wg6ZAFIM
[1]: https://github.com/shadowsocks/shadowsocks/tree/master
[2]: http://www.gigamonkeys.com/code-reading/
[3]: https://plato.stanford.edu/entries/computer-science/

What's the computer science? Have you ever asked yourself? It's not a science! 
Underlying our approach to this subject is our conviction that “com-
puter science” is not a science and that its significance has lile to do
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

# Components of cs
design(pattern) and algorithm, programming(debugging, hacking)
pl,assembly, compile(gcc), link, load(cll)
process, quantaties, order(synchronization)
turing, arch, os(boot, console, fedora, fs, process, mm, pm, time, virt, synchronization)
math, logic, electronics

OS Service | Hardware Support
------------|-------------------------------------
Protection  | Kernel/user mode, protected instructions, base/limit registers
Interrupts  | Interrupt vectors
System calls  | Trap instructions and trap vectors
I/O  | Interrupts and memory mapping
Scheduling, error recovery,accounting  | Timer
Synchronization  | Atomic instructions
Virtual memory  | Translation look-aside buffers
form | os
-----|----
change, computation, transfer . | instructions(memory,IO), Process, interrupt
relation, order | synchronization/atomic, scheduling, interrupt
quantaties, state | fs, memory, device, driver
# the advert of new world origin - performance
cache: in order to improve performace, cache tries to reduce the access time of transfer of future.
buffer: What does buffer do? temporary(Not future) place for data,reducing access time of transfer and 
maybe effiency of quantaties/times of transfer. 是确定的行为, 不同与cache的统计学原理.
time-sharing: multi-task from batch processing
prallel process: increas quantanties of process.

performance/efficiency
Easy to use
Security/Protection/isolation
Reliability
Energy-efficiency
# FAQ
[All about pseudo][10]
[10]: https://www.ibm.com/developerworks/library/os-aapseudo1/os-aapseudo1-pdf.pdf


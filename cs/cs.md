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

What's the computer science? Have ever asked yourself? It's not a science! 
[Richard Feynman on Computer Science — Talk at Bell Labs (1985)][0]
[Reading code? CODE IS NOT LITERATURE][2]
[The Philosophy of Computer Science][3]

# Reference
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

现在我们以新的视角看待os演化过程, 即以process/change, quantaties/state, relation/order这些natural origin为wilds,
辅以will意志的origin: 
performance/efficiency
Easy to use
Security/Protection/isolation
Reliability
Energy-efficiency

# kerenl
General subsystem: schedule, memory, fork, lock, vfs. April 4, 8 days
Network + redhat requirements. 8 days
KVM. source code level and hands-on lkml

# FAQ
[All about pseudo][1]
[1]: https://www.ibm.com/developerworks/library/os-aapseudo1/os-aapseudo1-pdf.pdf

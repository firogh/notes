---
tags: [ cs ] 
title: The art of programming
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Computer programming
[The practice of programming](http://www.cs.princeton.edu/~bwk/tpop.webpage/), [How to design programs](http://www.ccs.neu.edu/home/matthias/HtDP2e/), [SICP](http://sarabander.github.io/sicp/), and Elements of programming.
Reentrancy

# Concurrent programming
[Firo: must thinking summary: EW Dijkstra: Cooperating sequential processes](https://www.cs.utexas.edu/users/EWD/transcriptions/EWD01xx/EWD123.html)
CSAPP3e: Chapter 12 concurrent programming
Parallel and Concurrent Programming in Haskell
The origin of concurrent programming: from semaphores to remote procedure calls - Per Brinch Hansen
[Introduction on history of concurrency: Shared-Memory Concurrency](https://www.dcl.hpi.uni-potsdam.de/teaching/pvprog/Slides/C1_concurrency.pdf)
[Turing lecture: The computer science of concurrency: the early years](https://dl.acm.org/citation.cfm?id=2771951)
[Firo: example: Concurrency-managed workqueues](https://lwn.net/Articles/355700/)
[Rob Pike: Concurrency is the composition of independently executing computations.](https://talks.golang.org/2012/concurrency.slide#6)
## [Asynchrony (computer programming)](https://en.wikipedia.org/wiki/Asynchrony_(computer_programming))

## process vs event
[thread vs event](http://courses.cs.vt.edu/cs5204/fall09-kafura/Presentations/Threads-VS-Events.pdf)

# Lock-free programming
ring buffer, rcu

# Functional programming
[CPS in C](https://spin.atomicobject.com/2012/11/01/hey-c-is-a-functional-language-too/)
[Continuation-passing style](https://en.wikipedia.org/wiki/Direct_style)

# programming principles
[Programming principles](http://en.wikipedia.org/wiki/Category:Programming_principles)
High cohesion low coupling

# Design pattern
[Linux kernel design patterns](http://lwn.net/Articles/336224/)
[Linux Kernel Programming Patterns](http://www.cs.fsu.edu/~baker/devices/notes/patterns.html#)

# IPC
[Busy waiting vs blocking](http://www.cyberiapc.com/os/ipc_whatisit.htm)

# Project engineering
[kernel asm/asm-generic](https://lists.kernelnewbies.org/pipermail/kernelnewbies/2012-March/004986.html)
asm stands for arch specific macros(FIXME).

# Coding style
[GNU Coding Standards](http://www.gnu.org/prep/standards/standards.html)
比如GNU coding standards, Linux kernel coding style, Shell coding standard
* Label
It is considered to be safer that the label reflect what happens at the
destination, and not the reason for reaching the label. --Julia

# Good taste
remove_list_entry(entry)
{
  // The "indirect" pointer points to the
  // *address* of the thing we'll update
  indirect = &head;

  // Walk the list, looking for the thing that
  // points to the entry we want to remove

  while ((*indirect) != entry)
    indirect = &(*indirect)->next;

  // .. and just remove it
  *indirect = entry->next;
}

remove_list_entry(entry)
{
  prev = NULL;
  walk = head;

  // Walk the list

  while (walk != entry) {
    prev = walk;
    walk = walk->next;
  }

  // Remove the entry by updating the
  // head or the previous entry

  if (!prev)
    head = entry->next;
  else
    prev->head = entry->next;
}

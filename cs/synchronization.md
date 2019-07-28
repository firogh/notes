---
tags: [ kernel ] 
title: Synchronization
date: 2015-02-27T15:46:12+08:00 
category: cs
---

# Scalability
[How to Quantify Scalability](http://www.perfdynamics.com/Manifesto/USLscalability.html)
[Scalability Techniques for Practical Synchronization Primitives](http://queue.acm.org/detail.cfm?id=2698990)

# Definition
[Synchronize](http://www.etymonline.com/index.php?term=synchronize)
A program is synchronized if all accesses to shared data are ordered by synchronization operations.
A data reference is ordered by a synchronization operation if, in every possible
execution, a write of a variable by one processor and an access (either a read or a
write) of that variable by another processor are separated by a pair of synchroni-
zation operations, one executed after the write by the writing processor and one
executed before the access by the second processor???. Cases where variables may
be updated without ordering by synchronization are called data races because the
execution outcome depends on the relative speed of the processors, and, like
races in hardware design, the outcome is unpredictable, which leads to another
name for synchronized programs: data-race-free.
## Why we need synchronization
A critical section is a piece of code that accesses a shared resource 
(data structure or device) that must not be concurrently accessed by more than one thread of execution.
This overlap, where the result depends on the relative timing of multiple tasks, is called a race condition. 

# What is synchronization in computer science 
Process synchronization refers to the idea that multiple processes are 
to join up or handshake at a certain point, in order to reach an 
agreement or commit to a certain sequence of action.
* Mutual exclusion -- only one excution routine in  critical section
Readers–writers problem
The Producer-Consumer Problem 
* Cooperative
TCP Handshake 
* Deadlock
Dining philosophers problem
* Starvation
* Prioirty inversion
* Busy waiting

# Memory barier

# Per-CPU

# Local Interrupt Disabling
local_irq_disable() used in the code path that never disabled interrupts.
local_irq_save(flags) used in the code path that already disabled interrupts.

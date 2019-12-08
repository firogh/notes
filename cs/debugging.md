---
tags: [ cs ] 
title: The art of debugging
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Quote
Everyone knows that debugging is twice as hard as writing a program in the first place. So if you're as clever as you can be when you write it, how will you ever debug it?
"The Elements of Programming Style", 2nd edition, chapter 2. Brian Kernighan

# Reference
Debug hacks: a book on how to debug.

# Bug classifications
[Software bug types](https://en.wikipedia.org/wiki/Software_bug#Types)
[CWE VIEW: Research Concepts](https://cwe.mitre.org/data/definitions/1000.html)
[CWE VIEW: Simplified Mapping](https://cwe.mitre.org/data/definitions/1003.html)
[CWE VIEW: Development Concepts](https://cwe.mitre.org/data/definitions/699.html)
## Hardware Bugs
If this has only happened on a single physical machine, I suggest that machine be considered to be faulty.
## Memory corruption
[The generic term "memory corruption"](https://cwe.mitre.org/data/definitions/119.html)is often used to describe the consequences of writing to memory outside the bounds of a buffer, when the root cause is something other than a sequential copies of excessive data from a fixed starting location(i.e., classic buffer overflows or CWE-120). This may include issues such as incorrect pointer arithmetic, accessing invalid pointers due to incomplete initialization or memory release, etc.]
[An example by Neil Brown: The corrupted list of inodes could be due to one inode being freed and re-used while still on the list - or it could be due to memory corruption of a forward pointer.](https://bugzilla.suse.com/show_bug.cgi?id=1155930#c12)
Memory corruption is one of the most intractable class of programming errors, for two reasons:
The source of the memory corruption and its manifestation may be far apart, making it hard to correlate the cause and the effect.
Symptoms appear under unusual conditions, making it hard to consistently reproduce the error.
[Memory corruption](https://en.wikipedia.org/wiki/Memory_corruption)
[Memory safety](https://en.wikipedia.org/wiki/Memory_safety)
uninitialized memory: [wild pointer](https://en.wikipedia.org/wiki/Dangling_pointer#Cause_of_wild_pointers)
use after free: [dangling pointer](https://en.wikipedia.org/wiki/Dangling_pointer#Cause_of_dangling_pointers)
buffer overflow:
unknown source memory corruption: The generic "memory corruption".
memory leak:
### Phonomenon
Invalid page fault(including NULL pointer dereference)

# Debugging
[Abductive reasoning](https://en.wikipedia.org/wiki/Abductive_reasoning#Logic-based_abduction)
Every bug belons to a known type.
## Steps according to observations, narrow down Hypothses, successive approximation
Scientific method
## Get observations and analysis data
observation.log
Data: whole structure, list corruption;life cycle; which part is ok and which part is wrong, Data connections
call paths: related
## Think backward
How far from symptom to root cause could we think?
Missing cause:
## Known bug types assumptions 
Mix up Bug types, observations and related all paths

## Connect to programming skills.

# Anti-debugging
* Syntax checking
gcc -Wall
bash -n
* static code analysis
smatch

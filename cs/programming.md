---
tags: [ cs ] 
title: The art of programming
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Basic conceptions
Reentrancy

# CPS
https://spin.atomicobject.com/2012/11/01/hey-c-is-a-functional-language-too/
https://en.wikipedia.org/wiki/Direct_style

# Reference
[Programming principles](http://en.wikipedia.org/wiki/Category:Programming_principles)
programming principles
High cohesion low coupling

# Design pattern
[Linux kernel design patterns](http://lwn.net/Articles/336224/)
[Linux Kernel Programming Patterns](http://www.cs.fsu.edu/~baker/devices/notes/patterns.html#)

# IPC
[Busy waiting vs blocking](http://www.cyberiapc.com/os/ipc_whatisit.htm)

# Computer programming
[The practice of programming](http://www.cs.princeton.edu/~bwk/tpop.webpage/), [How to design programs](http://www.ccs.neu.edu/home/matthias/HtDP2e/), [SICP](http://sarabander.github.io/sicp/), and Elements of programming.

## Procedure and Subroutine
https://en.wikipedia.org/wiki/Link_register
[Dijkstra, E. W. (1960). "Recursive Programming"](https://link.springer.com/article/10.1007%2FBF01386232)
[Subroutine and procedure call support](https://people.cs.clemson.edu/~mark/subroutines.html)
https://people.cs.clemson.edu/~mark/subroutines.html
https://en.wikipedia.org/wiki/Subroutine
* 1945 Turing on subroutines in [Proposed electronic calculator.](http://www.alanturing.net/turing_archive/archive/p/p01/P01-011.html)
In Chapter 6. Outline of Logical Control.
We also wish to be able to arrange for the splitting up of operations into...
When we wish to start on a subsidiary operation we need only make a note
of where we left off the major operation...
* 1952 [The use of sub-routines in programmes](http://www.laputan.org/pub/papers/wheeler.pdf)
The above remarks may be summarized by saying sub-routines are very useful — although not absolutely necessary — and that the prime objectives to be born     in mind when constructing them are simplicity of use, correctness of codes and accuracy of description. All complexities should—if possible—be buried out of sight.

# Files sytle
[kernel asm/asm-generic](https://lists.kernelnewbies.org/pipermail/kernelnewbies/2012-March/004986.html)
asm stands for arch specific macros(FIXME).
# Coding style
[GNU Coding Standards](http://www.gnu.org/prep/standards/standards.html)
比如GNU coding standards, Linux kernel coding style, Shell coding standard
* Label
It is considered to be safer that the label reflect what happens at the
destination, and not the reason for reaching the label. --Julia

# Verification and validation
抽象的知识来自生活经验的总结, 所以学习抽象知识的重要方向是从实践不断总结抽象.
programming is a goal-oriented activity
14.4 
Before attempting to solve a problem, make absolutely sure you  what the problem is.
14.5
Before developing a program, make precise and refine the pre/postcondition.
program -> 
{Q} S {R}: predicte, formal notation
Q: input asseration
R: output asseration 
predictes -> asseration
asseration: a predicate placed in a program is called an asseration.
Proof outline: a program together with an asseration between each pair of statements
Program specification ⊃ excution ∪ speed ∪ size 
Instances: summation, squre root approximation, sorting
command-comment ⊃ all input ∪ output
# Lock-free programming
ring buffer, rcu

# Debugging
see debugging.md
# Testing
[Google testing blog](http://googletesting.blogspot.com/)
1.Black-box testing test software function.
2.White-box testing test software internal logic.

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

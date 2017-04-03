---
tags: [ cs ] 
title: The art of programming
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Reference
[Programming principles](http://en.wikipedia.org/wiki/Category:Programming_principles)
programming principles
High cohesion low coupling

# Computer programming
编程作为我重要的谋生手段, 有必要审慎的加以考察审视. 程序设计应该包含那些最少的, 必备的, 重要的能力呢?
[The practice of programming](http://www.cs.princeton.edu/~bwk/tpop.webpage/)给出了大概的框架, 但不完整.
我认为最重要的就是Abstraction and design.
[How to design programs](http://www.ccs.neu.edu/home/matthias/HtDP2e/)还有[SICP](http://sarabander.github.io/sicp/)以及Elements of programming.
其次我们要掌握几门Programming language.
养成好的Coding style, 
Valid and verification
Testing
Debuging

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
# Debugging
see debugging.md
# Testing
[Google testing blog](http://googletesting.blogspot.com/)
1.Black-box testing test software function.
2.White-box testing test software internal logic.
kvm & supermin

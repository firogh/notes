---
tags: cs
title: c
date: 2015-02-27 15:46:14
category: cs
---
#COMPILER, ASSEMBLER, LINKER AND LOADER: A BRIEF STORY
http://www.tenouk.com/ModuleW.html

#Book
C Traps and Pitfalls
Expert C Programming


#linkage
Happened in link(ld) step.
Link source object and library.(so-called environment.)
Internal linkage refers to everything only in scope of a translation unit. 
External linkage refers to things that exist beyond a particular translation unit.

#lexical pitfall
# Greedy lexical analysis

* Write tokens with blank!
x = y/*p;  /* oops, hidden error.*/


* Associative
You donot need to remember it! Just use it!

* Precedence
Need to remember, but if you not sure, parenthess.

#Common concepts
* Every C variable declaration has two parts: a type and a list of expression-like things called declarators.
* A declarator looks something like an expression that is expected to evaluate to the given type.
* C compiler to translate C programs into terms that make sense to the linker.
* One important job of a linker is to handle these name conflicts.
* Expression?
* Satement?

#Bitfiled with endianess
http://yarchive.net/comp/linux/bitfields.html
http://mjfrazer.org/mjfrazer/bitfields/
http://www.naic.edu/~phil/notes/bitfieldStorage.html
http://www.linuxjournal.com/article/6788?page=0,0

#FAQ
* Logical operation with signed value? x86!
int c = 0xFFFFFFFF;  int d = c >> 31; => d == f;
unsigned c = 0xFFFFFFFF;  int d = c >> 31; d!= f;
* shift beyond bits long?
c >> 296 == c >> 8

* Diffences between strlen() and sizeof()?
char *s, s1[]; are same to strlen(), sizeof(s) == 8 in x86_64, sizeof(s1) == stlen + 1; one for \0

* C volatile?
volatile int c= 1;
c = c; //?

* C const
const int *x; //Is a bad declartion!
int const *x; //Is a good declarion!

+ Below codes no Warning!!!
    int z =1;         
    int const *y =&z ;
We can conclude that const int *, the type specifier and qualifiers only apply to declarator y! not z!



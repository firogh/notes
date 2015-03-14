---
tags: cs
title: The programming language
date: 2015-02-27 15:46:14
category: cs
---
# The c programming language
##compiler, assembler, linker and loader: a brief story
http://www.tenouk.com/ModuleW.html

##Book
C Traps and Pitfalls
Expert C Programming

##linkage
Happened in link(ld) step.
Link source object and library.(so-called environment.)
Internal linkage refers to everything only in scope of a translation unit. 
External linkage refers to things that exist beyond a particular translation unit.

##lexical pitfall
## Greedy lexical analysis

* Write tokens with blank!
x = y/*p;  /* oops, hidden error.*/
* Associative
You donot need to remember it! Just use it!
* Precedence
Need to remember, but if you not sure, parenthess.

##Common concepts
* Every C variable declaration has two parts: a type and a list of expression-like things called declarators.
* A declarator looks something like an expression that is expected to evaluate to the given type.
* C compiler to translate C programs into terms that make sense to the linker.
* One important job of a linker is to handle these name conflicts.
* Expression?
* Satement?

##Bitfiled with endianess
http://yarchive.net/comp/linux/bitfields.html
http://mjfrazer.org/mjfrazer/bitfields/
http://www.naic.edu/~phil/notes/bitfieldStorage.html
http://www.linuxjournal.com/article/6788?page=0,0

##FAQ
* Logical operation with signed value? x86!
int c = 0xFFFFFFFF;  int d = c >> 31; => d == f;
unsigned c = 0xFFFFFFFF;  int d = c >> 31; d!= f;
* shift beyond bits long?
c >> 296 == c >> 8
* Diffences between strlen() and sizeof()?

#ASM
bdi 4, 8 delay solt
mips instruction size is fixed, 32bit, 4byte.
instruction address:  instrction in hex formate	   instruction in string formate, 260
8002c28c:   8c440104    lw  a0,260(v0)

##lwr & lwl
load a word
different with endianess
register 63.....32......0
big endian: lwl high bits in b + 0
little endian: lwl high in b + off
向中心

##endianess
ar71xx big
ralink little
cpu -> toolchain

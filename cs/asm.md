---
tags: mips
title: asm
date: 2015-02-27 15:46:14
category: mips
---
bdi 4, 8 delay solt


mips instruction size is fixed, 32bit, 4byte.

instruction address:  instrction in hex formate	   instruction in string formate, 260
8002c28c:   8c440104    lw  a0,260(v0)

#lwr & lwl
load a word
different with endianess
register 63.....32......0
big endian: lwl high bits in b + 0
little endian: lwl high in b + off
向中心

#endianess
ar71xx big
ralink little

cpu -> toolchain


---
tags: [ cs ] 
title: Computer architecture
date: 2015-10-13
category: cs
---

# Computer architecture
[Who are the Computer Architects?]https://people.cs.clemson.edu/~mark/architects.html
The Mathematical Theory of Communication
Given a symbol level, the architecture is the description of the system in  whatever system-description scheme exists next below the symbol level. - Newell, 1990, p. 81
Digital Design and Computer Architecture 2nd Edition
Computer Organization and Design 5th Edition
Structured Computer Organization 6th Edition
Write Great Code: Volume 1: Understanding the Machine
## Subfields
[Architectural state](https://en.wikipedia.org/wiki/Architectural_state)
Microarchitectural state, such as information stored in TLBs and caches

# Circuits 
A Symbolic Analysis of Relay and Switching Circuits

# Memory hierarchy
[Latency Numbers Every Programmer Should Know](http://people.eecs.berkeley.edu/~rcs/research/interactive_latency.html)

# CPU
[How a CPU Works](https://youtu.be/cNN_tTXABUA)

## ISA
ISA: memory model, registers, data types, instructions, word size(?).
Memory model: unit of address resolution, word, aligment, address space, addressing mode, memory barrier/memory order primitive's semantics.

# I/O
serial communication: UART(16550) + RS-232 
parallel communication: SCSI, ISA, ATA, PCI, FSB

## Data struct alignment
struct foo { char c; int i;};
[UNALIGNED MEMORY ACCESSES](https://www.kernel.org/doc/Documentation/unaligned-memory-access.txt)
[A bug story: data alignment on x86](http://pzemtsov.github.io/2016/11/06/bug-story-alignment-on-x86.html)
[The Lost Art of C Structure Packing](http://www.catb.org/esr/structure-packing/)
[Typical alignment of C structs on x86](http://en.wikipedia.org/wiki/Data_structure_alignment#Typical_alignment_of_C_structs_on_x86)
### Reasons for not alignment
 1. Casting variables to types of different lengths, e.g. char * to int *
 2. Pointer arithmetic followed by access to at least 2 bytes of data , 不太理解.
### Programming skills
Reorder members of struct;
get/put_unaligned to avoid analigned access.
attribute aligned
### Calculate the sizeof of aligned c struct
Data alignment means putting the data at a memory address equal to some multiple of the word size, which increases the system's performance due to the way the CPU handles memory.
1. find the widest scalar member and attribute( aligned(x)) to determin alignment.
2. fill the member to alignement without wrap
3. Pading to alignment

## Endianess and bitfield
[The order of allocation of bit-fields within a unit](https://gcc.gnu.org/onlinedocs/gcc/Structures-unions-enumerations-and-bit-fields-implementation.html)
It's Determined by ABI not Gcc. Check comments on [Bitfield endianness in gcc](https://stackoverflow.com/questions/47600584/bitfield-endianness-in-gcc)
### [How Endianness Effects Bitfield Packing](http://mjfrazer.org/mjfrazer/bitfields/)
if defined(__LITTLE_ENDIAN_BITFIELD)
    __u8    ihl:4,
        version:4;	# MSB, check wikipeida ipv4 header
### GCC bug on bitfield
[Betrayed by a bitfield](https://lwn.net/Articles/478657/)

# DMA
Check books COD, COA and ULK 3rd

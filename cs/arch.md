---
tags: [ cs ] 
title: Computer architecture
date: 2015-10-13
category: cs
---
# ref
https://people.cs.clemson.edu/~mark/architects.html

# ASM
Reference asm global label in C
char _text[];           x86_64_64;
unsigned long _text;    x86_64_PC32
They are different in amrithmatic operstion.

##Reference
[Latency Numbers Every Programmer Should Know](http://people.eecs.berkeley.edu/~rcs/research/interactive_latency.html)
[How a CPU Works](https://youtu.be/cNN_tTXABUA)
A Symbolic Analysis of Relay and Switching Circuits
The Mathematical Theory of Communication
Given a symbol level, the architecture is the description of the system in 
whatever system-description scheme exists next below the symbol level. - Newell, 1990, p. 81
[Ask HN: How to learn about the history of computing?](https://news.ycombinator.com/item?id=9844090)
《模拟电子技术基础 童诗白 第四版》第一章前半部分
Structured Computer Organization 6th Edition
Digital Design and Computer Architecture 2nd Edition
Computer Organization and Design 5th Edition
Write Great Code: Volume 1: Understanding the Machine
See MIPS run
Intel 64 and IA-32 architectures software developers manual combined volumes 3A, 3B, and 3C: System programming guide	
Microelectronics

# Definition
[Architectural state](https://en.wikipedia.org/wiki/Architectural_state)
microarchitectural state, such as information stored in TLBs and caches

## ISA
model: 1570s, "likeness made to scale; architect's set of designs," from Middle French modelle 
Semantics in computer science: In programming language theory, semantics is the field concerned 
with the rigorous mathematical study of the meaning of programming languages.
Language primitive
ISA: memory model, registers, data types, instructions, word size(?).
Memory model: unit of address resolution, word, aligment, address space, addressing mode, memory barrier/memory order primitive's semantics.
#x86 Interrupt
If interrupt occured in user mode, then cpu will context swith for potential reschedule.
The Interrupt Descriptor Table (IDT) is a data structure used by the x86 architecture to implement an interrupt vector table. 
##Hardware interrupts
are used by devices to communicate that they require attention from the operating system.
asynchronus
more details in init_IRQ() or set_irq() in driver.
##software interrupt 
synchronus
more details in trap_init().
* exception or trap
is caused either by an exceptional condition in the processor itself, 
divide zero painc?
* special instruction, for example INT 0x80
or a special instruction in the instruction set which causes an interrupt when it is executed.
# I/O IC 
serial communication: UART(16550) + RS-232 
parallel communication: SCSI, ISA, ATA, PCI, FSB


# Data struct aligment
[A bug story: data alignment on x86](http://pzemtsov.github.io/2016/11/06/bug-story-alignment-on-x86.html)
[The Lost Art of C Structure Packing](http://www.catb.org/esr/structure-packing/)
[Typical alignment of C structs on x86](http://en.wikipedia.org/wiki/Data_structure_alignment#Typical_alignment_of_C_structs_on_x86)
## 为什么需要结构体对齐?
struct foo {
char c;
int i;};
如果是32位, cpu 一次取4byte a word 数据.
如果我们把i的前3byte和c存到一起, 剩下1byte of i自己单独存.
那么我们访问i这个数据就要读两个4byte a word. 对cpu来说性能损耗.
如果我们把i单独放到4byte 对齐的地址, 那么我们只需要一次cpu读取.fast!
## 产生非对齐访问的场景
 1. Casting variables to types of different lengths, 比如char * 到int *
 2. Pointer arithmetic followed by access to at least 2 bytes of data , 不太理解.
## 我们做什么?
* 什么也不干, 按默认对齐来Natural alignment
* 为了不影响性能, 同时减少内存使用, 编程时最好显示reorder.
* get/put_unaligned  to avoid analigned access.
* 通过attribute aligned指定对齐要求.
* 数据要在不同体系, 32/64之间使用, 比如网络,写到disk, 我们必须要attribute packed
也就是说不对齐, 不同平台对齐可能不同, 我们不能让数据corruption.
## 如果数据不对齐有什么, cpu怎么办?
[必读UNALIGNED MEMORY ACCESSES](https://www.kernel.org/doc/Documentation/unaligned-memory-access.txt)
1. 如果用了packed, 编译器会生成extra代码阻止非对齐访问, performance loss.
2. cpu呢? 可能正确处理raise a exception to fix it with performance loss.
## Calculate the sizeof of aligned c struct
Data alignment means putting the data at a memory address equal to some multiple of the word size, 
which increases the system's performance due to the way the CPU handles memory.
1. find the widest scalar member and attribute( aligned(x)) to determin alignment.
找到结构体内scalar和attribute最大的对齐要求.
2. fill the member to alignement without wrap
把结构的成员一次填满对齐宽度, 不够的填到下个对齐宽度, 空出来留着padding
3. Pading to alignment
填上所有空.

# Endianess and bitfield
[The order of allocation of bit-fields within a unit](https://gcc.gnu.org/onlinedocs/gcc/Structures-unions-enumerations-and-bit-fields-implementation.html)
It's Determined by ABI not Gcc. Check comments on [Bitfield endianness in gcc](https://stackoverflow.com/questions/47600584/bitfield-endianness-in-gcc)
## [How Endianness Effects Bitfield Packing](http://mjfrazer.org/mjfrazer/bitfields/)
if defined(__LITTLE_ENDIAN_BITFIELD)
    __u8    ihl:4,
        version:4;	# MSB, check wikipeida ipv4 header
## GCC bug on bitfield
[Betrayed by a bitfield](https://lwn.net/Articles/478657/)

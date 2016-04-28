---
tags: [ cs ] 
title: Computer architecture
date: 2015-10-13
category: cs
---

##Reference
[How a CPU Works](https://youtu.be/cNN_tTXABUA)
Legacy 常用寄存器，常见指令 实模式和保护模式 分段和分页机制 TSS和任务管理 中断机制 时钟机制 高速缓存
《模拟电子技术基础 童诗白 第四版》第一章前半部分
Structured Computer Organization 6th Edition
Digital Design and Computer Architecture 2nd Edition
Computer Organization and Design 5th Edition
Write Great Code: Volume 1: Understanding the Machine
See MIPS run
Intel 64 and IA-32 architectures software developers manual combined volumes 3A, 3B, and 3C: System programming guide	
Microelectronics

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


# Endianess
[Endianness: Big and Little Endian Byte Order](http://www.yolinux.com/TUTORIALS/Endian-Byte-Order.html)
应该说bit endianess 实际存储只有MSB ... LSB这一种二进制表达形式! 在上面的文章的representtion, 辅证这一点. 
这篇文章[再谈C语言位域](http://tonybai.com/2013/05/21/talk-about-bitfield-in-c-again/), 
之所以输出逆序, 是因为错误里理解了, bit 序. bit 序是cpu 读取的方式, 不是存储的方式! 
* [bit endianness](https://en.wikipedia.org/wiki/Bit_numbering)
这样就可以更好的理解iphdr:
\#if defined(__LITTLE_ENDIAN_BITFIELD)
    __u8    ihl:4,
        version:4;
\#elif defined (__BIG_ENDIAN_BITFIELD)
    __u8    version:4,
        ihl:4;
\#else
\#error  "Please fix <asm/byteorder.h>"
\#endif
ipv4 header的拓扑
version ihl 
0100 0101
因为le的cpu是 lsb first, 也就是从最左面的1开始读, 所以第一个是ihl
而be的cpu是 msb first, 从最右开始所以version开头.
而使用unsigned char ver_ihl
(ver_ihl & 0xf0)>> 4 = version
ver_ihl & 0x0f = ihl
是无关cpu读取顺序的.

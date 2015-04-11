---
tags: cs
title: Hardware
date: 2015-02-27 15:46:14
category: cs
---
# Hardware
常用寄存器，常见指令
实模式和保护模式
分段和分页机制
TSS和任务管理
中断机制
时钟机制
高速缓存

#CPU
##Reference
Intel 64 and IA-32 architectures software developers manual combined volumes 3A, 3B, and 3C: System programming guide	
See MIPS run

##Register

## Address
## Interrupts
## Timer
## TLB
##CPU cahe
A CPU cache is a cache used by the central processing unit (CPU) of a computer 
to reduce the average time to access data from the main memory.
### Associativity - CPU Cache algorithms
* Two-way set associative cache
### Cache coherence
cache coherence is the consistency of shared resource data that ends up stored in multiple local caches.
* MSI
###Cache type
* Instructin cache
* Data cache
* TLB - Translation lookaside buffer

##Endianess
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
###Endianess in build
cpu -> toolchain
ar71xx big
ralink little

#86
##Register
* gs
The linux kernel uses GS to access cpu-specific memory.

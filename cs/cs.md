---
tags: [ cs ] 
title: Computer science a kernel programmer's perspective
date: 2015-02-27T15:46:14+08:00 
category: cs
---
# Reference
[Vijay Kumar B's Embedded Programming with the GNU Toolchain](http://www.bravegnu.org/gnu-eprog/)

# The Architecture of Computer science
* Process 
Algorithm, TOC, Design Pattern, SICP, Logic, Mathematics
Programming: language, coding style
Compile, link, and load or interpret:
OS
Arch

# Process
CS研究Process. 通过Computation体现Process.
TOC研究computation, algorithm研究process的具体过程.
DP研究process的合理表现,组合形式.
Logic, Mathematics作为世界之源. 

* Top method
Abstruction
Combination
Virtualization
Exchange time and space
Isolation
Modular manner: D

* Top goal
Easy to use
Efficiency
Protection
Reliability
Security
Energy-efficiency

# Programming
Programming langueage: c, python, shell
Programming tools: vim
Compile Link: ELF
Testing
Debuging
Interface

# OS
Batch processing -> Time-sharing
Overlaying 
* vm
There are some great historical papers and books we should read before fully understanding virtual memory.
[Computer Structures: Readings and Examples ](http://research.microsoft.com/en-us/um/people/gbell/CGB%20Files/Computer%20Structures%20Readings%20and%20Examples%201971.pdf)
[Computer Structures: Readings and Examples html version](http://research.microsoft.com/en-us/um/people/gbell/Computer_Structures_Principles_and_Examples/contents.html)
Chapter 10 One-level storage system is the first implemention of virtual memory mind.
## Process management
进程的定义和PCB，进程和线程的区别，进程的三个基本状态及它们之间的转换关系，进程的同步，竞争和死锁，进程间通信
###Representation 
* Program memory
Stack(User/Kernel)
Heap
Data segment(data/bss)
Code segment
* PCB
Resource
Processor Context
Process state
###daemonize
http://fixunix.com/unix/84640-daemon-controlling-terminal.html
## Memory managerment
分页式管理，分段式管理，虚拟内存的概念，页面置换算法，内存分配算法
### Paging
paging is one of the memory management schemes by which 
a computer stores and retrieves data from the secondary storage for use in main memory.
* Page fault
###Page replacement algorithm
OPT
FIFO
Second-chance
LRU
### x86 memory segmentation
linux 基本不用
[Linux中的段](http://oss.org.cn/kernel-book/ch02/2.3.7.htm)
* GDT
* TSS
[Use of TSS in Linux](https://en.wikipedia.org/wiki/Task_state_segment#Use_of_TSS_in_Linux)
* Linear address
### [Virtual memory](https://en.wikipedia.org/wiki/Virtual_memory#Paged_virtual_memory)
It maps memory addresses used by a program, called virtual addresses, into physical addresses in computer memory.
* Logic/Virtual address
* Page table
### Memory allocation
* Buddy memory allocation. 
* Slab allocation/Memory Pool
## Device management
中断的概念，中断处理，I/O控制方式，缓冲区管理，设备驱动，磁盘调度和高速缓存
## Network stack
Protocol
## I/O
[Methods for designing a CPU's I/O interface generally fall into one of the following categories:](http://www.cs.uwm.edu/classes/cs458/Lecture/HTML/ch11s02.html)
Completely separate memory and I/O. buses DMA?
Common buses, separate control lines. Port-I/O
Common buses and control line. Memroy-maped I/O
###Higher level implemention of I/O
device or partion of device/memory -> file 
io -> stream
####[stream](https://en.wikipedia.org/wiki/Stream_(computing))
* [Standard streams](https://en.wikipedia.org/wiki/Standard_streams)
interface is stdio library or the kernel version.
* codata
### Low I/O type
* Programmed I/O/Polling
* DMA
* Interrupt I/O
* Channel I/O
### I/O scheduling
Elevator algorithm
###Asynchronous I/O NEED CLEAN
* synchronous I/O multiplexing and I/O event notification facility
select/poll/epoll
For the ease of use, the select loop is implemented as an *event loop* with callbacks.
libevent and libev is a well-designed *event loop*.Check shadowsocks for using of libev.
## File system 
文件的概念，文件的管理，文件系统
## System calls
系统调用的概念，系统调用的处理，系统调用类型
##CPU-device I/O
### Memory-mapped I/O
ioremap: physical address->logical address, simlar to vmalloc except we need not page.
### Ported-mapped I/O
##Non CPU-device I/O
### I/O channels
## 同步与异步IO
今天我们要辨析一下同步和异步IO. 我们先解释最基础的概念, 之后用生活化的例子
完成认知.
首先是blocking 和 non-blocking这两个概念. 这两个概念实质上是和IO没有关系.
他们是在说, 比如读数据, 如果没有数据我该怎么办. 也就是说, 他是在IO不存的时候,
在语义上才是有效, 如果你要读的数据始终存在, 那么你还会考虑阻塞与不阻塞的问题吗?
那你应该考虑什么? 同步还是异步IO, 倒地什么是同步或者异步呢?
英文synchronous, syn 和chronous构成, syn是在一起的意思而chronous是时间的意思.
也就是说在一个时间点上在一起, 那么是谁和谁在意一起呢?其中一个是IO可以肯定, 另外一个
就是执行IO的发起者, 通常也就是进程. 简单说来这个IO是由进程执行的.
那么异步IO呢, asynchronous是a + synchronous. a表否定, 我们知道在IO进行的过程中我们的
进程是始终存在的, 也就是说IO 和进程共享着相同的时间进度, 但是却不在一起.也就是说,
IO不是由我们的进程完成而是别的进程完成, 是谁呢,是内核线程.
那么我们就知道只有linux上的aio是符合异步IO的标准, 而多路复用, 如epoll返回是我们和IO是在
一起, 我们要调用read之类的完成他.
下面用一个生活化的例子, 说明下linux下的IO.
比如你要买<百年孤独> 还要买件衬衫, 你会怎么买
你要先去书店, 如果店家没有货, 如果一个小时后货补齐了你可以等,
长点你就不愿意等了. 也就是阻塞和不阻塞.之后你要去衣服点.也是同样.
假如都没货, 你有等不下去, 怎么办? 你又不能一会儿去这家问下书怎么样? 一会儿去衣服店询问.
这太累人了. 最好的方式, 你告诉店家我要什么, 到货了给我打个电话, 我在过来.
这就是io多路复用. 可是还有更高明的方法, 没错就是京东了, 我网上下订单, 第二天直接送到家了.
京东, 为什么成功? 你知道了吧.用户省心啊.对应异步io

# Computer Architecutre
∅ ⊢ Computer
Computer ⊃ CPU ∪ Memory ∪ Bus ∪ Peripheral 
Computer → I/O

∅ ⊢ CPU
CPU ⊃ Instruction set architecture ∪ Microarchitecture ∪ ?
Instruction set architecture ⊃ Memory model ∪ ISA Registers ∪ Data types ∪ Instructions
Instruction set architecture(extension)  ⊃ RISC ∪ CISC  
Memory model ⊃ Byte ∪ Words ∪ Address space ∪ Memory semantics 
ISA Registers ⊃ Special-purpose registers ∪ General-purpose registers ∪ Control registers
Data types ⊃ Numeric ∪ Nonnumeric
Numeric data types ⊃ Integer
Instructions ⊃ Instruction formats ∪ Instruction length ∪ Addressing mode ∪ Comparisons ∪ Branches ∪ Procedure Call 
Instruction formats ⊃ opcode
Instructions(extension) ⊃ Data transfer ∪ Arithmetic ∪ Logic ∪ Control flow

RISC ⊃ Fixed instruct length ∪ Few instruction formats ∪ Load/Store architecture

Microarchitecture ⊃ Data path ∪ Control path ∪ MMU ∪ CPU cache ∪ Oscillator crystal ∪ Clock generator
Microarchitecture → Instruction cycle ∪ Instruction pipeline ∪ Out-of-order
Instruction pipeline ⊃ Branch prediction ∪ Hazard ∪ Parallel
Data path ⊃ ALU ∪ FPU ∪ Registers ∪ Bus
ALU ⊃ Adder ∪ Multiplexor
Adder ⊃ Logic gate
Logic gate ⊃ diodes or transistors
Transistor ⊃ 

MMU ⊃ TLB

CPU register ⊃ not same with SRAM ∪ Synchronous circuit

CPU cache ⊃ SRAM ∪ Synchronous circuit 
SRAM ⊃ MOSFET ∪ Computer organization and design page B-59
CPU cache → Cache coherence ∪ Associativity ∪ Cache miss
Cache coherence ⊃ MSI

Oscillator crystal 

Clock generator ⊃ frequency divider ∪ clock multiplier
Clock generator → Clock rate

∅ ⊢ Memory
// No matter physical or virtual address both are relative continuity.
// What is memory? Simplely, a large number of identifier and 1 byte infomation.
// What is address space is the methmatical set of identifiers of memory.
Memory ⊃ Sequencial logic circuit ∪ Memory controller ∪ Bank 
Memory controller ⊃
∅ ⊢ Bus
∅ ⊢ Peripheral

Digital circuits
Analog circuits

# Physical computation phenomenon
A Symbolic Analysis of Relay and Switching Circuits
The Mathematical Theory of Communication
Given a symbol level, the architecture is the description of the system in 
whatever system-description scheme exists next below the symbol level. - Newell, 1990, p. 81
[Ask HN: How to learn about the history of computing?](https://news.ycombinator.com/item?id=9844090)
* Control
Cogwheel control
electromechanical magnet plugging control
control sequence points


# Faq
* How does gcc attribute((aligned)) work?
struct S1 { short f; short f1; short f2;char a; char c;} __attribute__ ((aligned ));
sizeof S1 = 16 in 64-bit
* In what situation can unaligned accesss make a kernel panic?
may be arch/mips/kernel/unaligned.c
* Is the address generated by compiler physical or virtual?

Graphviz + CodeViz

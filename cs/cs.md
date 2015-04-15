---
tags: cs
title: Computer science a kernel programmer's perspective
date: 2015-02-27 15:46:14
category: cs
---

#Epistemology
石里克 <普通认识论>
胡适<实用主义>
Faq list
Incessancily continuously Share your what read, thought
GEB
* s3c24xx@gmail.com
凭什么我们就如此受制于人or 书？他就能搞懂，而我就不行呢？”。
其实，我们需要的是一种重其意而忘其形的根本之道，
需要的是一种兵来将挡，火来水淹的通用解决方法。
而绝不是淹没于牛人们的结论中。
+ 否则，遇到一个新的问题，就只能埋怨牛人的书还不够厚，以至于没把你需要的东西也包括进去了。

#Mathematics 
In mathematics, and more specifically in algebra, a domain is a ring such that ab = 0 implies a = 0 or b = 0.

#Automata
Logic and Mathematics -> Turning machine and automata -> computer

#Logic
* Completeness, this one is most important part!
* Consistency
* Soundness 一一性

# Language
More details in programming language notes

#Algorithm
Algorithms: Design Techniques and Analysis

#Design pattern
Publish-Subscribe Mechanism 
observation


# Hardware
More details in hardware notes 

# OS
## Process management
进程的定义和PCB，进程和线程的区别，进程的三个基本状态及它们之间的转换关系，进程的同步，竞争和死锁，进程间通信
###Representation 
* Programe  memory
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

##Device management
中断的概念，中断处理，I/O控制方式，缓冲区管理，设备驱动，磁盘调度和高速缓存
###Low I/O type
* Programmed I/O/Polling
* DMA
* Interrupt I/O
* Channel I/O
###I/O scheduling
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

#I/O
CPU and main memory is the brain of a computer.
Any transfer of information to or from the CPU/memory combo, for example by reading data from a disk drive, is considered I/O. 
##CPU-device I/O
### Memory-mapped I/O
### Ported-mapped I/O

##Non CPU-device I/O
### I/O channels

##Interface
[Methods for designing a CPU's I/O interface generally fall into one of the following categories:](http://www.cs.uwm.edu/classes/cs458/Lecture/HTML/ch11s02.html)
Completely separate memory and I/O. buses DMA?
Common buses, separate control lines. Port-I/O
Common buses and control line. Memroy-maped I/O
###Higher level implemention
device or partion of device/memory -> file 
io -> stream
####[stream](https://en.wikipedia.org/wiki/Stream_(computing))
* [Standard streams](https://en.wikipedia.org/wiki/Standard_streams)
interface is stdio library or the kernel version.
* codata

#Buffer
a data buffer (or just buffer) is a region of a physical memory storage 
used to temporarily store data while it is being moved from one place to another.
* Page cache, Buffer head
* Pipe

#Cache
a cache is a component that stores data so future requests for that data can be served faster; 
the data stored in a cache might be the results of an earlier computation, 
or the duplicates of data stored elsewhere. 
##CPU cache
##GPU cache
##Disk cache
##Web cache

#Scheduling
## Process scheduler
## Network scheduler
## I/O scheduling

#File formate
##ELF -- ELF executable and linkable format.
Used for Relocatable file(object file, kernel moudle), Executable file, Dynamic library, Core dump. 
###Relocatable file(object file, kernel module)
ELF header.
Sections.
Section header table.
### Executeable file, dynamic library
ELF header.
Program header table.
Segments.
Section header table.

# Net
More details in net notes 

#Testing
##Black-box testing
test software function.
##White-box testing
test software internal logic.

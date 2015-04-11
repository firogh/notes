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

# Net
More details in net notes 

# Hardware
More details in hardware notes 

# OS
## Process management
进程的定义和PCB，进程和线程的区别，进程的三个基本状态及它们之间的转换关系，进程的同步，竞争和死锁，进程间通信
###daemonize
http://fixunix.com/unix/84640-daemon-controlling-terminal.html

## Memory managerment
分页式管理，分段式管理，虚拟内存的概念，页面置换算法，内存分配算法

##Device management
中断的概念，中断处理，I/O控制方式，缓冲区管理，设备驱动，磁盘调度和高速缓存

## File system 
文件的概念，文件的管理，文件系统

## System calls
系统调用的概念，系统调用的处理，系统调用类型

#Testing
##Black-box testing
test software function.
##White-box testing
test software internal logic.

#Asynchronous io NEED CLEAN
##Common concepts
a hard drive seeking a track to read or write; this is often orders of magnitude slower than the switching of electric current.
* The simplest software solution. polling-based system.
##Forms
* Prcesses, pipeline
* synchronous I/O multiplexing and I/O event notification facility
select, poll
epoll
For the ease of use, the select loop is implemented as an *event loop* with callbacks.
##libevent and libev is a well-designed *event loop*.
Check shadowsocks for using of libev.

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

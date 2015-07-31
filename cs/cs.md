---
tags: cs
title: Computer science a kernel programmer's perspective
date: 2015-02-27 15:46:14
category: cs
---
# Reference
[Code The Hidden Language of Computer Hardware and Software](http://book.douban.com/subject/1494026/)


计算机科学是什么? 包括那些内容? 那些内容对我来说重要?
这些问题我都没有认真的思考过. 对于一直希望成为一名romote kernel programmer的
我来说, 内核就是一切吗? 从事内核工作的和崔添翼在Jane street的Ocaml, 又有什么区别呢?
和那些从事openembedded开发, openstack, python, php之类有什么不同呢?还有和汇编开发有什么区别呢?
还有搞算法,数学一类的工作有什么差异, 这些内容都不是绝对互斥的. 还有Makefile, automake这些技术.
还有编译技术gcc automake, 又如何呢? 还有解bug这种没有具体形式的工作. 
相同的地方都是本质都是思维运作, 算法和数学最能体现出这点.
不同点是什么呢? 语言! 还有特质化的内容.

我们来看另外一个问题, 如何才能算上出类拔萃的程序员, 如果让崔添翼来搞内核, 会是我现在这个样子吗?
这些年嵌入到内核的函数调用是对的吗? 有那么值吗?是否有更好的方法成为一个优秀的内核开发者呢?
优秀的内核开发者应该具备那些素质呢?

回到上面列举的计算机科学的种种技术上来, 我们可以肯定, 所有的技术, 框架都是被所谓的计算机的语言所描述的!
通过语言的方式呈现在我们面前.
assembly language, c, make, python php lisp ocaml等等.
所以计算机语言是作为计算机技术的基础, 但并不是核心.
人类语言是对世界的描述, 而计算机语言是对计算机的描述, 他们很类似.

同为计算机语言的assembly language 和c python lisp这些语言有什么差异呢?
可以说, 作为所有其他语言基石的汇编语言, 值得我们去一探究竟.
assembly language 经过汇编编程machine code.
machine code 类似shell脚本, 而cpu类似shell解释器.
先看看gnu as的实现. You can find it in:
git://sourceware.org/git/binutils-gdb.git
核心是md_assemble, 果然复杂, 但是用过objdump都知道汇编语句和机器吗有种几乎直接对应的关系.
我们还是再从语言的层面来挖掘汇编.
为什么叫assembly?


## FIXME concepts
code: 手电筒 ->　莫尔斯码
Numeral system: 手指头　-> 二进制
logic: 三段论 -> bool -> 电路开关 亚里士多德 布尔 香农
A Symbolic Analysis of Relay and Switching Circuits
The Mathematical Theory of Communication
触发器 英国射电物理学家 William Henry Eccles
晶体管

#Epistemology
石里克 <普通认识论>
胡适<实用主义>

* s3c24xx@gmail.com
凭什么我们就如此受制于人or 书？他就能搞懂，而我就不行呢？”。
其实，我们需要的是一种重其意而忘其形的根本之道，
需要的是一种兵来将挡，火来水淹的通用解决方法。
而绝不是淹没于牛人们的结论中。
否则，遇到一个新的问题，就只能埋怨牛人的书还不够厚，以至于没把你需要的东西也包括进去了。

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

# Algorithm
Algorithms: Design Techniques and Analysis
kernel/algorithm.md

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
A CPU cache is a cache used by the central processing unit (CPU) of a computer 
to reduce the average time to access data from the main memory.
### Associativity - CPU Cache algorithms
* Two-way set associative cache

### Cache coherence
Cache coherence is the consistency of shared resource data that ends up stored in multiple local caches.
Cache conerence is mantained by mostly archtecure like cpu
* MSI

###Cache type
* Instructin cache
* Data cache
* TLB - Translation lookaside buffer
##GPU cache
##Disk cache
##Web cache

# Data struct aligment
[The Lost Art of C Structure Packing](http://www.catb.org/esr/structure-packing/)
[Typical alignment of C structs on x86](http://en.wikipedia.org/wiki/Data_structure_alignment#Typical_alignment_of_C_structs_on_x86)
## 为什么需要结构体对齐?
struct foo {
char c;
int i;
};
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
3. pading to alignment
填上所有空.

## 关于kernel中put/get_unaligned实现
access_ok, do nothing in essence
byteshift, 移位每次访问u8, 
packed_struct: 交给gcc
memove, byte-wise copy

## Faq
* How does gcc attribute((aligned)) work?
struct S1 { short f; short f1; short f2;char a; char c;} __attribute__ ((aligned ));
sizeof S1 = 16 in 64-bit

* In what situation can unaligned accesss make a kernel panic?
may be arch/mips/kernel/unaligned.c

#Scheduling
## Process scheduler
## Network scheduler
## I/O scheduling

#Endianess
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

# Hardware
常用寄存器，常见指令
实模式和保护模式
分段和分页机制
TSS和任务管理
中断机制
时钟机制
高速缓存
##Reference
See MIPS run
Intel 64 and IA-32 architectures software developers manual combined volumes 3A, 3B, and 3C: System programming guide	

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


---
tags: [ cs ] 
title: Computer science a kernel programmer's perspective
date: 2015-02-27T15:46:14+08:00 
category: cs
---
# Reference
[Vijay Kumar B's Embedded Programming with the GNU Toolchain](http://www.bravegnu.org/gnu-eprog/)

# The Architecture of Computer science
SICP states that cs focuses on process.
What process is is the set of changes and quantaties.而这些changes 和 quantaties live in model of computation.
我们可以说一个process就是一个从属于model of computation的changes和quantaties的一次排列和集合.
所以model of computation至少是change的集合 和 quantaty的集合的集合.processess 本身就是暗含着data/quantaties的.
Why can  processes being? It's model of computation.
这就是cs的form, process和model of computation. What is the material of cs?
cs 的material 是symbolic.
For now, we have noticed that the form of cs is changes and quantities.
And the material of cs is symbolic(or, symbolic system).
Both form and material are properties of being/abyss.
人类的思维使用多种materials(vision, animation, graphics,or, feelings)去express a concept。
可以说程序设计语言就是标准描述 process的Semiotic system。
所以， pl对process的elements or origins做了合适的描述。
sicp研究的符号规则层面的process 和 data。 design pattern研究的是语义层面的。
PL 给了我们一种视角, 从process的元素的角度去描述process.
form language(BNF->syntax, regex->lexical)藐视pl的语法. 
form language可以通过automata鉴别.automata等价于form system lambda calculus.
自动机被相应的form grammar分类.
我们的目的是规范化人类思维的表达形式.以此来达到交流, 具现化.
我们要明白, pl的所有内容都是围绕the set of changes and quantaties建立的, 
也就是说pl是能指, 而所指便是the set of changes and quantaties.
syntax句法不是我们关注的, 我们关注的是semantic.从semantic角度去尽可能的描述changes和quantaty.
quantaties and changes: type system
我们给quantaties附上特定的类型, 表达特定的含义, 
primitive expression: 数学量, 符号学字符, function
combination: compound elements
abstraction: name an elements
relation: inference, relaton of changes, Flow Diagrams, Turing Machines And Languages With Only Two Formation Rules; 
也就是Jhone Locke的binary relation.
process 可以看成人的意志力展示.origins加入interence不关注changes. logic就是研究过程/changes和过程/changes之间的关系.
control flow描述设定的就是这种order/relation.核心不在于有没有if else, 而是是否有足够的表达能力.
表达changes之间的relation/order.可能是一种图的感觉, 就比如if else引起了tree这种order的产生.
也就是说语言的表达力. changes要类似automata的四通八达的感觉.
process由 changes, inference/relation, quantaties组成.因为这是由人的意志will 参与的process.
inference 强调的是order of changes/computation,也就是control flow.
Why does the order of execution matter?
Then we need programming:sicp c3(c2/c1)
then interpreter: sicp c4
link load
computation model: sicp c5
if else: 表示的是relation(changes之间的关系, order, causality, inference)
也就是说pl包含:
changes, quantaties, relation. 这几种表述.
pl和machine/model of computation近乎等价, progrm是machin的超集. program 是process.而pl不是.
所以pl和machine都对process有表达的能力.pl和machine的form一致, 但material不同. 他们都是process元素的集合.
programming是排列组合这些process elements的craft.可以说这些元素是data, 可以转变为process.
这里面我们就定义programmer的skills, crafts, 是一种排力组合的能力, 不严禁的通俗的理解.
design pattern 更注重宏观process. 他们排布process. 单这些process是处理别的process.high-order process(procedure?)
而algorithm, 可以认为是关注的是primitive/ process.
图灵机加入的"感知",正是laking of causal.
之于表述computational process, 认为(hypothesis?)自然语言和pl是具有相同能力的.
programming的目的之一是把思维结果转为pl形式表达出来.
另外, 我们现在能总结出, cs的两大主题就是:the form of process and how to express more better.
process的内在, 以及如何表达process.cs剩下的问题都是programmer 码农的问题.
现在我们来研究纯粹的process.继承sicp的定义.
procedure是process的体现. process >= procedure. 所谓的local evolution, 等价于c,lisp的function.
procedure 是一changes(quantaties, relation)有限的集合.
一个process可以包含多个procedure, 也可以只有一个procedure(如, 递归, 迭代).
还有另外一个就是data.process和data唇亡齿寒.
process: procedure, computation/changes.
data->: compound data, quantaty. 
我们研究的重点似乎时relation吧?
process和data既像一物两面, 又像唇亡齿寒.
下面开始, 
从combination, abstraction, relation这三个方面属性的process.
从简单到复杂, 先关注process.
如果process只有一个procedure.  elementary process.
似乎我们可以得到一个关于process的坐标系: procedure 的种类和数量.
这里, 我们不关注algorithm.
这里, 关注design pattern, GoF, elemental design pattern, POSA.
从坐标系角度只考虑process和process之间的关系:
1 只有一种quality的procedure, 完成的process.
那么数量上如果构成大于1, 也就是procedure call procedure.
可以是iteration or recursion; 我门的坐标系反应不出来.
需要另外一个属性, procedure 自身是否是运算的参数.
坐标系改下, procedure的数量换成procedure是否参与运算. 如果参与computation就是递归.
默认procedure 数量>=2.
我们再一次发现, procedures之间可能并不是直接调用的.
所以一个小坐标系, 能表达的太有限, 要废弃掉.从属性角度看.但可以明确procedures出现次数是2个以上.
当然, 我们是从process角度分析问题的.
只有一种procedure, 
* call/order relation
参与 computaton 是 recursive, otherwise, iteration.
iter(a,i, n)
if (i<n)
 a = a * (a +1)
 i++;
 return iter(a, i, n)//tail call>
看来这个问题的实质是process是否作为计算的对象.而recursive和iteration是某个general的特例.
loose relation: sequence(特例iteration), 是computation之间的关系.
strict relation: computation是内在的关系. computation被抽象data
primitive computation可以调function, function可以调primitive computation.
computation function 经过抽象变成data.data反过来又可构成computation.
可以说没有data, 全是computation!?那这么说(所谓的"recursive", 就司空见惯了.)
function反应的是computation 序列集合.自身调用自身, 首先满足这种 order sequence的关系.
似乎称之为recursive是没问题的. 但一个只能反应sequence, or和sequence等价的"recursive".
又有多大价值呢?recursive意义又有多大呢? 究竟, process的核心是什么呢? 是computation.
回答显示世界的recursive, 一个画面不断重复自身的画面!
![recursion](https://upload.wikimedia.org/wikipedia/commons/6/62/Droste.jpg)
我们发现, 他们总是在表达一个完整的个体.
画面的recursion, 依然在一个画面下.
function的recursion, 确实表现在一个function内.
但是从computation角度, recursive的function表述的是computation的连续sequence.
也就是说recursion是有对象的,对于process是computation是对象.
对于recursion process, 只有一个computation, 而iteration的process是多个computation的order/sequence集合.
所以要区分computation和procedure/function. function是computation和sequence集合.他反应的是computation之间的关系.
并不能反应computation自身内在关系.这也是我们混淆了recursion procedure的原因.
想想, 集合的复合还是computation sequence. 而computation自身的复合却是一个computation,自身的演变.
让我们体悟到了function和computation的区别.不同对象的交互operation差异.
对函数的讨论研究等价与, 讨论computation之间的关系.也就是把function全部替换成了computation.
所以recursive function就成了不断内嵌sequence computation了, 递归全无.
computation 内在的组合, 我们讨论了, 相似对象/function的情况.
如果一个computation内部,是由不同computation组成的.就是非递归的情况,edp叫Conglomeration.合理
computation内在讨论先停下.
我们关注computations之间的关系.
无论是computation的内部还是,computation之间data都成了纽带.
我们只讨论具有relation的process/computation.
如果通过共同的操作的data,
这就是并发的问题, 也就是changes.
如果共同的data, 不是作为操作的对象知识表达一个relation.
这是能想到的通过data的processes/computations之间的交互.
似乎量changes/computation/process 之间的relation, 只能是
causality and synchronization?(FIXME).
Causality: Observer(发起者主动, 结果者被动), polling(结果者主动)
state pattern(发起者的状态), Strategy Pattern(根据对象的选择行为)
Command Pattern(?decoupling, 貌似就是个Observer不过把状态给了接收者处理),
Mediator Pattern(decoupling), 
design pattern 关注的是form, syntax.
研究process/procedure/function/computation 到 data
 Simple Factory Pattern类似strategy pattern, 根据(接收者类型产生对象)
Factory Method Pattern类似(Mediator pattern, 将对象生成放给子类)
没有relation(or relationless)的process联系到一起.
adapter pattern
还有一种两个procedure concurrent, 要分开 order sequence化.
concurrent是design pattern等价的.
这样design pattern也融合进来了.

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

It has been 5.5 years since I first touch  the light of computer science.
More over, I have been dicided to become a linux kernel hacker for a long time. 
Saddly, I am still not a real kernel hacker. 
For this moment, it's the appropriate time for me to catlog a sumary what computer scince from my view.

Yes, it maybe fuzzy to other reader. The book <a href="http://csapp.cs.cmu.edu/">CSAPP</a> is great book I have been read. By giving the respect to CSAPP, I named this article with CSAKPP :-)

<图片1>

###Epistemology
Learning by using! because of  unfeasible and mind.
Faq list
incessancily continuously Share your what read, thought
GEB

### Formal language
##regular expression
Firstly, I would like to point out that regular expressions are equivalent to finite state machines. Secondly, I would certainly say that regular expressions are (at least have a way to transform into) formal grammars.

###Design pattern
Publish-Subscribe Mechanism 
observation

###Algorithm
Algorithms: Design Techniques and Analysis



###Programming tool
* vim
Learning the vi Editor
* Make
http://www.gnu.org/software/make/manual/make.html
* gdb
http://www.gnu.org/software/gdb/documentation/
* gcc
http://gcc.gnu.org/onlinedocs/gcc/Attribute-Syntax.html
* diff

* patch

* english

* scapy

### Language
##C
Expert c programming
C Traps and Pitfalls

C Programming FAQs
http://c-faq.com/

##Python
+ simpler starter

##Assembly
Linux GNU assembly

###Revision control
##base concepts
##git
Write and Submit your first Linux kernel Patch
http://www.youtube.com/watch?v=LLBrBBImJt4

##svn

###Protocol
http://www.securitywizardry.com/packets/pdf/

###Proxifier


###Compiler
##GCC 
#Extensions to the C Language Family maybe is useful to understand and write some kernel code. Please, do not try to remeber all the extension of gcc, look up when you meet or need it.
http://gcc.gnu.org/onlinedocs/gcc-4.9.0/gcc/index.html

# Collect Common gcc Errors and Their Meanings
It will notablely decrease the time of elimit your  compile-time errors.
http://web.ics.purdue.edu/~cs240/misc/errors.html


###Infrastructure-tools
##net tools
create net dev interface
nmap

##daemonize
http://fixunix.com/unix/84640-daemon-controlling-terminal.html


##Iptables
https://www.frozentux.net/iptables-tutorial/iptables-tutorial.html

###kernel reference
http://lwn.net/
http://elinux.org/Kernel_Debugging_Tips
https://www.kernel.org/doc/
http://kernelnewbies.org/KernelJanitors/Todo
##traning or challenge to kernel hacker
http://eudyptula-challenge.org/


###kernel debug
##static code analysis
smatch

###Kernel Infrastructure

##Lockdep

##prarllel programming
ftp://kernel.org/pub/linux/kernel/people/paulmck/perfbook/perfbook.html

## Process management
进程的定义和PCB，进程和线程的区别，进程的三个基本状态及它们之间的转换关系，进程的同步，竞争和死锁，进程间通信
##MM
分页式管理，分段式管理，虚拟内存的概念，页面置换算法，内存分配算法
##Device management
中断的概念，中断处理，I/O控制方式，缓冲区管理，设备驱动，磁盘调度和高速缓存

##fs
文件的概念，文件的管理，文件系统

##Proc fs
##mkdir 
proc_net_mkdir


##system calls
系统调用的概念，系统调用的处理，系统调用类型

##OOM
Taming the OOM killer

##Netifiler
nfconntrack
NAT

##Lock
lock original primitives 
Mutex mutual exclusion among concurrent threads regardless of whether they be readers or updaters 
Reader-writer locks that allow concurrent reads but not in the presence of updates 
RCU supports concurrency between a single updater and multiple readers. 
#RCU
http://www.rdrop.com/users/paulmck/RCU/
I think RCU is over.

###Kernel Networking stack
3 flow in networking stack 
hostedan netedan
parse header sequence
##Netlink sockets
Join community

##TCP

##ICMP

##IP
header


##netfilter
#conntrack
#nat
#xtables
writing your xtables

##Routing

##Neighboring system


##tc qdisc

##softirq

##irq

###想象力


###Hardware architecture
常用寄存器，常见指令

实模式和保护模式

分段和分页机制

TSS和任务管理

中断机制

时钟机制

高速缓存
http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html
Intel 64 and IA-32 Architectures Software Developer's Manual Combined Volumes 3A, 3B, and 3C: System Programming Guide

###Testing
##Black-box testing

test software function.

##White-box testing
test software internal logic.
#static testing tools
lint

#Opensource project
=GNU
* glibc coreutils gcc gdb-binutils make
=Kernel relating
* util-linux nl  iproute2 net-tools dracut git linux
=etc
* procps findutils iputils tcpdump



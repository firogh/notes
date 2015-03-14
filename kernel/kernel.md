---
tags: kernel
title: kernel
date: 2015-02-27 15:46:12
category: kernel
---
#Hacking
* How to find hooked functions
For example, all start_xmit of ppp_channel_ops, 
pppol2tp_chan_ops, pppol2tp_xmit,
pptp_chan_ops pptp_xmit
pppoe_chan_ops pppoe_xmit

grep -nr 'ppp_channel_ops' /path/to/{l2tp, pptp, pppoe}


#Reference
http://stackoverflow.com/questions/914693/learning-kernel-programming/27782191#27782191

*Book
...

* Website

#Content of kernel developing
* Attitudes & methods in kernel developing
	Nothing new under sun.

* kernel developing environment

* language
	English
	C
	shell
	python
	Make

* Tools
	vim
	gcc
	sed, awk

* The principles of OS
1. 进程管理
进程的定义和PCB，进程和线程的区别，进程的三个基本状态及它们之间的转换关系，进程的同步，竞争和死锁，进程间通信

2. 内存管理
分页式管理，分段式管理，虚拟内存的概念，页面置换算法，内存分配算法

3. 设备管理
中断的概念，中断处理，I/O控制方式，缓冲区管理，设备驱动，磁盘调度和高速缓存

4. 文件管理
文件的概念，文件的管理，文件系统

5. 系统调用
系统调用的概念，系统调用的处理，系统调用类型
关于操作系统理论方面的书籍，推荐Andrew S. Tanenbaum编写的《Operating Systems: Design and Implementation》一书。

* Hardware
Cpu, register, address
http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html
Interrupts
Timer
TLB

#What happened to kernel and me
This article is the my enssence answer  to "kernel".
How could I become a linux kernel hacker. The question is exist for a long long time.
And I didn't get the true answers, I read many book and web-page to understand kernel.
I am still not to get in touch with kernel hacking. I am just reading.
What happened to me?

In recently, I read Wittgenstein's book and got the philosophy of life.
I am not on the kernel-way that make me be very far from kernel.

What is kernel-way, it's the connecion, contributing, community.
We need to keep touch with others. Many programmer contribute their life kernel source code 
and make it stronger.

Finally, I get the answer, join with other, goto kernel community.

#What could I can do?
It's hard question to answer!
In my company, I do a little kernel hacking and *never* submit out modifications to kernel matainer!
It's so suck!
And more, I am really didn't good at on kernel hacking even though I have been many many boooks on kernel.
Based on the realities, I can't submmit the feature to kernel community!
I can fix BUGs!

#Fix bugs
=kernel oops
Yes, I can do it! I found kerneloops.org(It's hard to access, because of DB error.:-) store many kernel oops info.
It's the best work for me:
1. Connect with community.
2. Improve understanding of kernel.
It's so hard, I can't do anyone .....:-(((((((((

=kernel junitor and some other work
clean kernel code

=summary kernel knowledge and skill


=Write kernel code

#KVM
* KVM architecture
* Kernel running flow
  svm.ko -> svm_init(svm.c) -> kvm_init(kvm_main.c) -> 
* File
  svm.c vmx.c kvm_main.c kvm_svm.h 
* Function  
* Struct
  kvm_x86_ops x86.h
  vcpu_svm    kvm_svm.h  
 

#Signal
* struct signal_sturct: 
The "struct signal_struct" is the random *leftovers* from all the other stuff.
http://thread.gmane.org/gmane.linux.kernel/512831/focus=513990

* sigpending
Store blocked signal info

##Generate signal
__send_signal();

##Process siganl
* SIGKILL (may be some other)
process in _send_signal()-> complete_signal() tsk->state |= TASK_WAKEKILL 
http://lwn.net/Articles/288056/
http://www.ibm.com/developerworks/library/l-task-killable/

* others
each time a switch is made from kernel mode to user mode, 
arch-specific: entry.S -> do_siganl()
{ 
	get_signal_deliver()
	{
		if fatal -> do_greoup_exit()->...__cleanup_sighand()
	}

	handle_signal() -> k->u(hanle)-sigreturn->k
}

##FAQ
* Non-mask signal
SIGKILL, SIGSTOP

* __schedule()

#Netlink
* Group
enum rtnetlink_groups

##What is netlink
Networking related kernel configuration and monitoring interfaces.
* IPC between kernel and user spacess process.
ioctl

* prarts
	libnl
	libnl-route
	libnl-genl
	libnl-nf

* How many parts does libnl-route has?
Address,  links, neighboring, routing, TC

##Faq
* What is the shortage of libnl-route.
    skbedit action
    cgroup classifier
    tun/tap dev 
    gre tunnel dev 
    tc classifier/action


#Namespace

#Module
##pre-require
modules.alias
http://doc.opensuse.org/documentation/html/openSUSE_113/opensuse-reference/cha.udev.html

http://blog.chinaunix.net/uid-22954220-id-4380202.html
运行时, 插入u盘也是这样.

2. 内核自己加载比如缺少模块的时候 网络协议, fs
https://unix.stackexchange.com/questions/90027/what-is-the-sequence-loading-linux-kernel-module-on-startup-how-priority-is-set/90037#90037

3 rc 里面smartqos之类的.

##Load module into kernel
vmlinux.lds.h linker scipts include helper macros.


#Patch

#Data structures
* u32 __u32
__u32 is used for user-space. declare a variabe used by icotl. qosmark.
u32 is used for kernel.
deatils in ldd3e chapter 10

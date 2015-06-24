---
tags: kernel
title: kernel
date: 2015-02-27 15:46:12
category: kernel
---

# Reference
* Book
* Website
http://lwn.net/
http://elinux.org/Kernel_Debugging_Tips
https://www.kernel.org/doc/
http://kernelnewbies.org/KernelJanitors/Todo
http://eudyptula-challenge.org/

# Design pattern
[Linux kernel design patterns](http://lwn.net/Articles/336224/)
[Linux Kernel Programming Patterns](http://www.cs.fsu.edu/~baker/devices/notes/patterns.html#)

# Find bug
* Considered fault types
Block “To avoid deadlock, do not call blocking functions with interrupts disabled or a spinlock held.”
Null “Check potentially NULL pointers returned from routines”
Var “Do not allocate large stack variables (>1K) on the fixed-size kernel stack.”
INull “Do not make inconsistent assumptions about whether a pointer is NULL.”
Range “Always check bounds of array indices and loop bounds derived from user data.”
Lock “Release acquired locks; do not double-acquire locks.”
Intr “Restore disabled interrupts.”
Free “Do not use freed memory.”
Float “Do not use floating point in the kernel.”
Size “Allocate enough memory to hold the type for which you are allocating.”

##KSM

#Signal
* struct signal_sturct: 
The "struct signal_struct" is the random *leftovers* from all the other stuff.
http://thread.gmane.org/gmane.linux.kernel/512831/focus=513990
* sigpending
Store blocked signal info
* Non-mask signal
SIGKILL, SIGSTOP
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

## Need patch
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

#Data structures
* u32 __u32
__u32 is used for user-space. declare a variabe used by icotl. qosmark.
u32 is used for kernel.
deatils in ldd3e chapter 10

#Panic
kernel/kernel/panic.c

#init
##initcall

##disk 
subsys_initcall 4 genhd_device_init with base_probe{ request_module()}
module_init 6 ->init_sd->sync_schedule_domain(sd_probe_async

##cmdline
root= name_to_dev_t, mount_root in prepare_namespace
如果/init不能 sys_access, 则prepare_namespace,切换到真正的root=指定的设备上设备在sd_probe上初始化了.
systemd负责挂在文件系统, 切换.

#src-tree
include/linux: share with userspace
include/net: kernel stuff

# Assembly in kernel
为了简单! 内核修改特权寄存器和指令, 用汇编实现简单.
尽可能加速. c到汇编, 编译器相对保守, 



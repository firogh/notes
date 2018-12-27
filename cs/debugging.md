---
tags: [ cs ] 
title: The art of debugging
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# ref
https://www.youtube.com/watch?v=g7Qm0NpPAz4
Debug hacks: a book on how to debug.

# Boot issues
debug ignore_loglevel earlyprintk=ttyS0,115200, keep keep_bootcon initcall_debug dyndbg=+plmf

# Lockup
[kernle doc - Softlockup detector and hardlockup detector](https://www.kernel.org/doc/Documentation/lockup-watchdogs.txt)

# OOB
## SLUB Redzone


# Use after free
Record the owner, who free.

# Deadlock
Lockdep
[The kernel lock validator](https://lwn.net/Articles/185666/)
[Runtime locking correctness validator](https://www.kernel.org/doc/Documentation/locking/lockdep-design.txt)
https://lkml.org/lkml/2013/2/4/4
http://bbs.chinaunix.net/thread-4183696-1-1.html

# Print
## User mode print
define witcher(fmt, args...) do{FILE * fdebug=fopen("/tmp/d.log", "a+"); \
fprintf(fdebug,"%s,%s,%d:"fmt, __TIME__, __FUNCTION__, __LINE__, ##args);fclose(fdebug);} while(0)
* make specific
-s, -n, -p, --warn-undefined-variables
$(warning ...)

## Early print
putstr
early_printk 
Linux serial-port driver is interrupt driven, if irq-off console will not work!

## Dynamic print
Precondition: CONFIG_DYNAMIC_DEBUG
How to use it during kernel booting?
Use boot option:
dyndbg="file drivers/usb/host/ehci-hcd.c +p; file
drivers/usb/host/ehci-ppc-of.c +p" loglevel=8
loglevel=8 dyndbg="module ehci_pci +p; 
module ehci_hcd +p; module usbcore +p"
After booting:
/sys/kernel/debug/dynamic_debug/control
## Dev print
drivers/base/core.c
define_dev_printk_level 
THese functions are like pr_func
dev_debug 
# Tracing
[Linux Tracing Technologies](https://www.kernel.org/doc/html/v4.18/trace/index.html)

## User mode tracing
ltrace, strace, bash -x

[How does strace work?][1]
[1]: https://blog.packagecloud.io/eng/2016/02/29/how-does-strace-work/
PTRACE_PEEKDATA for showing pointer parameter(e.g. 2nd arg of read)

# Kprobes 

# Systemtap
[systemtap脚本分析系统中dentry SLAB占用过高问题](https://yq.aliyun.com/articles/131870)


# CONFIG_DEBUG_PAGEALLOC
check_poison_mem in alloc_pages
free_pages_prepare posion

# FS
lsof



# Kasan
setup_arch->kasan_init
[KernelAddressSanitizer a fast memory error detector for the Linux kernel](http://events.linuxfoundation.org/sites/events/files/slides/LinuxCon%20North%20America%202015%20KernelAddressSanitizer.pdf)
[kasan found stack out of bounds](https://github.com/zfsonlinux/zfs/pull/4708/commits/01709937be3c28a89eff83e0e657a72826947506)
[lwn The kernel address sanitizer](https://lwn.net/Articles/612153/)
[out of bounds](https://lkml.org/lkml/2014/12/3/128)
# Reference
Reverse engineering
[How debuggers work][1]
[1]: http://eli.thegreenplace.net/2011/01/23/how-debuggers-work-part-1

# Core dump
echo /tmp/core-%e-%p-%s-%c-%P > /proc/sys/kernel/core_pattern

# eBPF
https://lwn.net/Articles/740157/
[How to turn any syscall into an event: Introducing eBPF Kernel probes](https://blog.yadutaf.fr/2016/03/30/turn-any-syscall-into-event-introducing-ebpf-kernel-probes/)

# Gdb
signle step instruction
## Debug glibc
directory ~/ws/glibc-2.23/libio
set solib-search-path /usr/lib/debug/lib/x86_64-linux-gnu/

# info
Host OS name and version number.
I40e driver version number.
Host OS Linux Kernel log. 
VF driver version number.
NIC hardware model and PBA number. 
NIC Firmware NVM version number.

# From assembly to C
addr2line -f -C -a 0xxxx -e ooo.bin
addr2line -e vmlinux -i ffffffff81f04b7a ffffffff81508daa

# From ELF to C
pahole -C 'block_device' vmlinux

# Debugging and Bug types
Does anyone can tell me what is debugging? Debugging is [Abductive reasoning][1].
Theory + Observations => Explanation

# Bug classifications
[BUG type of Jim Gray][2],除了Bohrbug,其他都不太好解.
The National Vulnerability Database是一个非常有用的网站, 国内鲜少有人提及.尤其是,
他的Bug分类方式惊艳无比, 逻辑上非常严密, 就好比数学系统, 由公理系统推导而得.
NVD的Bug分类也采用类似的构建方式.[CWE Cross Section Mapped into by NVD][3] 
涵盖了所有常见的的Bug描述, 而且非常专业.大赞!wikipedia的条目就相形见绌了
[Common types of computer bugs in wikipedia][4].

# General debugging steps
Debugging的逻辑过程是[Abductive reasoning][1]. 我们以此进行推导.
T: Theory 也就是我们的background, cs的知识技术, 出问题的程序与代码.
O: Observations 是Bug发生的现场以及相应的log, 现象.
E: Explanation 是最终定位的结论.
E ⊆ T; O ⊆ T; E ⊃ (E ∩ O); 
if E = (E ∩ O); then debugging is done.
T, E 与 O理论上都是从∅开始,且O => E;
E: ∅ -> Bug type(可能经过是多种Bug types的过渡状态) -> Explanation 
很多时候O也会使T扩大.所以O成为debugging的关键.
下面严格区分T 和 O. 我们得到O就要不断地结合T进行理性分析.
> Another perespective, 
> the process of debugging is use O to minimize T to E instance.
> T is solutions space, E [locates][5] in T. O is the address to find E.

基于NVD的CWE我们可知, 收集observations 可能是development和deployment的各个节点.

### Get observations

* compilation
make kernel/sched.s V=1
readelf, objdump
c++filt 
./scripts/decodecode < Oops.txt
echo 
* load software
LD_TRACE_LOADED_OBJECTS=1 git 
ldd /usr/bin/git
* software imediately stop
Use atexit() register a stackdump or a wrapped print

* tips
If an page oops close to zero, for example 0xfffffff4
It maybe ERR_PTR(-12);
[Tips on debugging optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)
code reordering inlining Optimized-away variables Tailcall optimization
* oops
From ASM to c language
[lkml-Linus-Al-Viro-oops-debug](http://yarchive.net/comp/linux/oops_decoding.html)
__asm__(), 常量, loop codes format, char *, ip 

[1]: https://en.wikipedia.org/wiki/Abductive_reasoning#Logic-based_abduction
[2]: http://www.opensourceforu.com/2010/10/joy-of-programming-types-of-bugs
[3]: https://nvd.nist.gov/cwe.cfm
[4]: https://en.wikipedia.org/wiki/Software_bug#Common_types_of_computer_bugs
[5]: https://nvd.nist.gov/cwe.cfm#cweIdEntry-CWE-1

# 一些比较通用的调试工具
* gdb
gdb vmlinux /proc/kcore
p jiffies_64
text_addr=/sys/module/char-read-write/sections/.text
add-symbol-file /home/nkhare/char-read-write.ko $text_addr
print &((struct kmem_cache *)0)->offset

# Anti-debugging
* Syntax checking
gcc -Wall
bash -n
* static code analysis
smatch

# BUG made by me
* print_signal_info wrong pritk parameters position
        printk(KERN_NOTICE "K %d : %d -> %s %d %s %d\n", sig, q->info.si_code,
                ss[2], ss[3], task_tgid_vnr(r_t), task_tgid_vnr(r_p));
 Watch compile warning info can be avoid of this bug.
* spin_lock(sighand) invoke down_sem and cond_resched...
	__send_signal()
# Examples
* timer_list->function = NULL
我前几天定位了一个Softirq中timer corruption的问题, 现象是timer func是个非法地址.
首先, 一下子不能确认是由那种Bug type导致的.可能是Improper Restriction of Operations 
within the Bounds of a Memory Buffer 也可能是Use after free. 所以此时debug的关键所在
就是收集observations,也就是function的名字, 而我遇到的这个问题timer 被完全写坏了. 
可以用ftrace收集所有timer 的地址和function, 再从oops里面得到timer地址,回头找ftrace
log中记录的function, 从而定位问题. 稍后介绍, 如何通过expect抓屏幕log.
这个方法没成功, 时候分析知道是因为panic的瞬间出问题的timer的地址和function没来得及
记录到ftrace.怎么办? 我直接说了, 我想到一个办法, 把timer的空间增大, 增大的空间用来
保存function,在oops把这块内存打出来. 此法非常有用, timer的function一下子就找到了, 进而
确认了问题是use after free. 非常开心:-)这里的解释了如何收集observations的技巧.对待use
after free的问题是够了, 以后用到这类问题基本可以妙解了.

* module text address 
cat /sys/module/wmi/sections/.text 
cat /proc/modules | grep wmi
int bss_var;
static int hello_init(void)
{printk(KERN_ALERT "Text location .text(Code Segment):%p\n",hello_init);
static int data_var=0;
printk(KERN_ALERT "Data Location .data(Data Segment):%p\n",&data_var);
printk(KERN_ALERT "BSS Location: .bss(BSS Segment):%p\n",&bss_var);}



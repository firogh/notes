---
tags: cs
title: General debugging method
date: 2015-02-27 15:46:14
category: cs
---

#Reference
DWARF

#BUG
drivers/net/wireless/libertas/cfg.c:1115 lbs_associate() error: potential null dereference 'cmd'.  (kzalloc returns null)
drivers/net/wireless/rtlwifi/efuse.c:378 read_efuse() error: potential null dereference 'efuse_word'.  (kmalloc returns null)
drivers/staging/rtl8192e/rtllib_rx.c:1496 rtllib_rx() error: we previously assumed 'ieee' could be null (see line 1464)
vers/media/media-device.c:146 __media_device_enum_links() warn: check that 'pad' doesn't leak information (struct has a hole after 'index')
drivers/media/media-device.c:167 __media_device_enum_links() warn: check that 'link' doesn't leak information (struct has a hole after 'index')

# [BUG type of JimGray](http://www.opensourceforu.com/2010/10/joy-of-programming-types-of-bugs/)
##Bohrbug, can be reproduce.
##Heisenbug 不论你用多少的时间和精力来试图让bug重现，bug就是人间蒸发了
##Mandelbug 当bug产生的原因过于复杂而难以理解时，bug的出现也变得没有规律
##Schroedinbug


#[Common types of computer bugs](https://en.wikipedia.org/wiki/Software_bug#Common_types_of_computer_bugs)
## Arithmetic bugs
## syntax error
## Logic error
Incorrect Bounds-Checking
off-by-one bug
Skipping Null-Termination Issues
## Resource bugs
uninitialized/nonvalidated/corrupted pointer dereference.
Segmentation fault in userspace
Kernel oops,[When the kernel de-references an invalid pointer, it’s not called a segfault – it’s called an ”oops”.](http://neependra.net/kernel/Debugging_Kernel_OOPs_FUDCon2011.pdf)

踩内存
## Race condition bug
Multi-threading programming bugs(parallel problems)
* deadlock
## Interfacing bugs
## Performance bugs
## Teamworking bugs
## Vulnerable bugs
*unbounded memory manipulation functions
strcpy
* Non-Null Termination Issues
non terminaed string
*Formate string
[Format Strings attacker](https://www.owasp.org/index.php/Format_string_attack) or [Uncontrolled format string](https://en.wikipedia.org/wiki/Uncontrolled_format_string)
* integer issues
integer overflow
Signed Comparison Vulnerabilities

##Special BUG
[kenrel lockup](http://www.av8n.com/computer/htm/kernel-lockup.htm)

#Taxonomy of Kernel BUG
* [oops, WARN_ON, or kernel panic](http://fedoraproject.org/wiki/KernelBugClassification)
* [Source of BUG](http://fedoraproject.org/wiki/KernelBugTriage#Kernel_Bug_Classification), driver or subsystem and so on.

#BUG made by me
* print_signal_info wrong pritk parameters position
        printk(KERN_NOTICE "K %d : %d -> %s %d %s %d\n", sig, q->info.si_code,
                ss[2], ss[3], task_tgid_vnr(r_t), task_tgid_vnr(r_p));
 Watch compile warning info can be avoid of this bug.
* spin_lock(sighand) invoke down_sem and cond_resched...
	__send_signal()

# Anti debugging
gcc -Wall

# Typical debugging process -- step 1, 2, 3 maybe loop.
0. 合理性假设可能性, 最高的一系列原因!
1. Generating program states, 对于内核很多时候, 我们只能有一次机会产生调试的信息, 就是panic or oops那时, 不绝对.
2. examine program states 
3. track down the origin of the problem(s)
4. fix it

# 合理推判
## 我们的程序 or 系统, 库有问题.
## 证明, 注重因果性, 而不是相关性Correlation
充分: 因->果, 基本backtrace可以断定
必要: 果->因, 这个bug只是这个因造成的.

# Generating program/system states
## Printing
* 刘东log法 #define debugme(fmt, args...) do{FILE *fdebug=fopen("/d.log", "a+"); fprintf(fdebug,"%s,%s,%d:", __TIME_, __FUNCTION__, __LINE__);fprintf(fdebug, fmt, ##args);fclose(fdebug);} while(0)

* before kernel decompress
putstr

* early printk
Linux serial-port driver is interrupt driven, if irq-off console will not work!

* printk

## beyond printing
Use atexit() register a stackdump() function
dump_stack in kernel
ioctl/netlink
print signal This is just a hiwifi wonderful kernel patch #931


## Log
dmesg
syslog

 
## Debugging support in the kernel
更为宽泛意义上的printing 方法.
* kernel tracing
http://lwn.net/Articles/291091/
http://lwn.net/Articles/330402/
http://lwn.net/Articles/379903/
http://lwn.net/Articles/381064/
http://lwn.net/Articles/383362/
http://lwn.net/Articles/346470/

* CONFIG_DYNAMIC_DEBUG
pr_debug()/dev_debug()
<debugfs>/dynamic_debug/control

## SysRq
t, m

## /proc (specially /proc/sys/) and /sys

## Trace
strace
ltrace
ftrace in kernel
kgtp

## Debuger Gdb kdb kgdb
gdb /usr/src/linux/vmlinux /proc/kcore
    bt
    x/100a
thread apply all bt full
* How to use gdb debug loaded kernel module(maybe kernel its self)
gdb vmlinux /proc/kcore
core-file /proc/kcore
p jiffies_64
text_addr=$(cat /sys/module/char-read-write/sections/.text)
add-symbol-file /home/nkhare/char-read-write.ko $text_addr

* how to get module text address
firo@firo module$ cat /sys/module/wmi/sections/.text 
0xffffffffa023b000

firo@firo module$ cat /proc/modules | grep wmi
wmi 18820 0 - Live 0xffffffffa023b000

……
int bss_var;
static int hello_init(void)
{
printk(KERN_ALERT "Text location .text(Code Segment):%p\n",hello_init);
static int data_var=0;
printk(KERN_ALERT "Data Location .data(Data Segment):%p\n",&data_var);
printk(KERN_ALERT "BSS Location: .bss(BSS Segment):%p\n",&bss_var);
……
}
Module_init(hello_init);


## Kprobes
* kgdb
gdb
file vmlinux
set remotebaud 115200
target remote /dev/ttyS0

## lockdep

## tcpdump/wireshark

## lsof

## [kernel oops](https://www.kernel.org/doc/Documentation/oops-tracing.txt)/warn/panic
狭隘的认为oops等价于内存地址出问题了, oops 本质上是__die("Oops"
__die 却可以表明很多错误 "Bad pagetable", "Oops - badmode"   
arm_notify_die("Oops - undefined instruction" 等等..
oops 是超出programmer 之外的错误,属于不可控风险, 其实更危险比panic.
panic 则是programmer 感知到的是防御式编程assertion的体现.

## Coredump and /proc/kcore and kdump

## library dependencies of a ELF/bin
LD_TRACE_LOADED_OBJECTS=1 git 
ldd /usr/bin/git



##General case
###how to find or generate a backtrace
* gdb bt(Strongly, recommand), break continue bt
* backtrace() / dumpstack
* read soucecode
* print log 
* Timer backtrace, Just for funny, the foolish of me. 

# Examine program states
* Variable values
* Function calls
* Packet
* Backtrace
* Registers
* Stack

## Exmaine oops.txt
* use System.map to resolve the function name of IP
kallsyms replace ksymoops tools.

* codes in oops
./scripts/decodecode < Oops.txt

## demangle c++ symbols 
c++filt command
addr2line -f -C -a xxx -e ooo
/home/build/x/ab/mips-openwrt-linux-addr2line -C -f -e /data/logs/hwf-health-chk/debug-root/HC6361/0.9005.5384s/debugfs/tmp/data/usr/bin/aria2c  0x00607188
aria2::ZeroBtMessage::~ZeroBtMessage()

# Tracking down the origin of the problems
##  Different Origins
### Assemblly level
* Generat vmlinux Assemblly codes
objdump -S vmlinux
objdump -S char-read-write.ko

* Generat single sources file assemblly  codes
make kernel/sched.s V=1

## Track down
### "Wolf fence" algorithm 
二分调试大法

### From ASM to c language
[lkml-Linus-Al-Viro-oops-debug](http://yarchive.net/comp/linux/oops_decoding.html)
* expand inline function
* locate __asm__() 内嵌汇编, 能快速定位代码! 但很少! slhc_uncompress()
* 找常量!
* 找loop codes formate!
+ef8:   00a01021    move    v0,a1
 efc:   88440003    lwl a0,3(v0)
 f00:   24450004    addiu   a1,v0,4                                                                                                           
 f04:   98440000    lwr a0,0(v0)                                                                                                               
 f08:   00641821    addu    v1,v1,a0                                                                                                            
 f0c:   0064202b    sltu    a0,v1,a0                                                                                                             
+f10:   14a7fff9    bne a1,a3,ef8 <slhc_uncompress+0x444>                                                                                         
 f14:   00831821    addu    v1,a0,v1
###Track skills
####[Tips on debugging optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)
* code reordering
* inlining
* Optimized-away variables
* Tailcall optimization


### General track down case 
* If an page oops close to zero, for example 0xfffffff4
It maybe ERR_PTR(-12);

* Exit bug
Use atexit() register a stackdump() function
gdb,break bt

* smartqos custom qdisc - self inferrence
要自己推测除几种可能, 之后按着思路去找, 不能汪洋大海, 乱砍.

### A case of oops
Register IP: -> System.map /proc/kallsyms -> objdump -S char-read-write.ko




#FIXME/FIXIT


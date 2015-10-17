---
tags: cs
title: Debugging
date: 2015-02-27 15:46:14
category: cs
---

# Reference
Reverse engineering

# Contents
Bug types
Bug made by me
Anti-debuging
General debugging steps 
Get observations
Debug Kernel bug 

# Bug types
## Gernel Bug types
## [CWE - Common Weakness Enumeration](https://nvd.nist.gov/cwe.cfm)
## [BUG type of JimGray](http://www.opensourceforu.com/2010/10/joy-of-programming-types-of-bugs/)
Bohrbug, can be reproduce.
Heisenbug 不论你用多少的时间和精力来试图让bug重现，bug就是人间蒸发了
Mandelbug 当bug产生的原因过于复杂而难以理解时，bug的出现也变得没有规律
Schroedinbug
## [Common types of computer bugs in wikipedia](https://en.wikipedia.org/wiki/Software_bug#Common_types_of_computer_bugs)
Arithmetic bugs
syntax error
Logic error
> Incorrect Bounds-Checking
> off-by-one bug
> Skipping Null-Termination Issues
Resource bugs
> uninitialized/nonvalidated/corrupted pointer dereference.
> Segmentation fault in userspace
> Kernel oops,[When the kernel de-references an invalid pointer, it’s not called a segfault – it’s called an ”oops”.](http://neependra.net/kernel/Debugging_Kernel_OOPs_FUDCon2011.pdf)
> Buffer overflow/踩内存
> [Double kfree errors](http://lwn.net/Articles/174494/)
> The devm_* series functions introduce more double free error in driver code.
Race condition bug
> Multi-threading programming bugs(parallel problems)
> deadlock
Interfacing bugs
Performance bugs
Teamworking bugs
Vulnerable bugs
> unbounded memory manipulation functions
> strcpy
> Non-Null Termination Issues
> non terminaed string
> Formate string
> [Format Strings attacker](https://www.owasp.org/index.php/Format_string_attack) or [Uncontrolled format string](https://en.wikipedia.org/wiki/Uncontrolled_format_string)
> integer issues
> integer overflow
> Signed Comparison Vulnerabilities
Special BUG
[kenrel lockup](http://www.av8n.com/computer/htm/kernel-lockup.htm)
## Taxonomy of Kernel BUG
[oops, WARN_ON, or kernel panic](http://fedoraproject.org/wiki/KernelBugClassification)
[kernel oops](https://www.kernel.org/doc/Documentation/oops-tracing.txt)/warn/panic
狭隘的认为oops等价于内存地址出问题了, oops 本质上是__die("Oops"
__die 却可以表明很多错误 "Bad pagetable", "Oops - badmode"   
arm_notify_die("Oops - undefined instruction" 等等..
oops 是超出programmer 之外的错误,属于不可控风险, 其实更危险比panic.
panic 则是programmer 感知到的是防御式编程assertion的体现.
[Source of BUG](http://fedoraproject.org/wiki/KernelBugTriage#Kernel_Bug_Classification), driver or subsystem and so on.
#BUG made by me
* print_signal_info wrong pritk parameters position
        printk(KERN_NOTICE "K %d : %d -> %s %d %s %d\n", sig, q->info.si_code,
                ss[2], ss[3], task_tgid_vnr(r_t), task_tgid_vnr(r_p));
 Watch compile warning info can be avoid of this bug.
* spin_lock(sighand) invoke down_sem and cond_resched...
	__send_signal()
# Anti-debugging
## Syntax checking
gcc -Wall
bash -n
## static code analysis
smatch
# General debugging steps -- [Abductive reasoning](https://en.wikipedia.org/wiki/Abductive_reasoning)
T + O => E; //Theory + observations => explanation
E is the sub-set of T, O is the result of E under the T.
The process of debugging is use O to minimize T to E instance.
0. Reproduce?
1. Find the bug type and definations what the bug belong to.
The bug type is the broad outline of the Expaination of the specific bug's Observation.
2. 结合实际环境get more observations and deuce the explanation/cause.
3. Fix it.
# Get observations
## Get observations from excute binary
DWARF
ELF header, readelf -h
Program header table, readelf -l
Segments, readelf --segments 
Sections, readelf --sections
Section header table, readelf -S 
objdump -S
## Get Observations from program
> #define debugme(fmt, args...) do{FILE *fdebug=fopen("/tmp/d.log", "a+"); \
> fprintf(fdebug,"%s,%s,%d:"fmt, __TIME__, __FUNCTION__, __LINE__, ##args);fclose(fdebug);} while(0)
Before kernel decompress use putstr
Linux serial-port driver is interrupt driven, if irq-off console will not work!
early_printk vs printk
dump_stack
ioctl/netlink
Use atexit() register a stackdump or a wrapped print
### Debuger Gdb kdb kgdb
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
* how to get the offset of member in struct
gdb ./vmlinux 
print &((struct kmem_cache *)0)->offset
## Kprobes
* kgdb
gdb
file vmlinux
set remotebaud 115200
target remote /dev/ttyS0
## Get observations from application
lsof
strace
bash  -x for shell
coredump
## Get observations from library
ltrace
library dependencies of a ELF/bin
LD_TRACE_LOADED_OBJECTS=1 git 
ldd /usr/bin/git
## Get observations from kernel
dmesg
SysRq
/proc (specially /proc/sys/) and /sys
ftrace
http://lwn.net/Articles/291091/
http://lwn.net/Articles/330402/
http://lwn.net/Articles/379903/
http://lwn.net/Articles/381064/
http://lwn.net/Articles/383362/
http://lwn.net/Articles/346470/
CONFIG_DYNAMIC_DEBUG
pr_debug vs dev_debug
<debugfs>/dynamic_debug/control
print signal This is just a hiwifi wonderful kernel patch #931
kgtp?
lockdep
kdump
* how to get module text address
firo@firo module$ cat /sys/module/wmi/sections/.text 
0xffffffffa023b000
firo@firo module$ cat /proc/modules | grep wmi
wmi 18820 0 - Live 0xffffffffa023b000
int bss_var;
static int hello_init(void)
{printk(KERN_ALERT "Text location .text(Code Segment):%p\n",hello_init);
static int data_var=0;
printk(KERN_ALERT "Data Location .data(Data Segment):%p\n",&data_var);
printk(KERN_ALERT "BSS Location: .bss(BSS Segment):%p\n",&bss_var);
……}
Module_init(hello_init);

## Observations of network
tcpdump netstat iptables wireshark
## Kernel specific
kernel version
ask reporter for the .config
decoding "code"


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

## Examine oops.txt
* use System.map to resolve the function name of IP
kallsyms replace ksymoops tools.

* examinecodes in oops
./scripts/decodecode < Oops.txt

echo "Code: 23 25 dc 47 6f 00 41 f6 c4 10 75 66 9c 5d fa 65 48 8b 14 25 a8 d1 00 00 48 8b 03 48 8d 04 02 4c 8b 28 4d 85 ed 74 55 48 63 53 18 <49> 8b 54 15 00 48 89 10 55 9d 4d 85 ed 74 06 66 45 85 e4 78 22" | ./kernel/scripts/decodecode 

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
为了使汇编代码和C代码更好的对应起来， Linux内核的Kbuild子系统提供了这样一个功能： 任何一个C文件都可以单独编译成汇编文件，例如：
make kernel/sched.s V=1

## Track down
### "Wolf fence" algorithm 
二分调试大法

##From oops to ASM
* Fast way maybe???
1. make kernel/xx.s
2. grep "code in oops.txt" kernel/xx.s

### From ASM to c language
[lkml-Linus-Al-Viro-oops-debug](http://yarchive.net/comp/linux/oops_decoding.html)
* expand inline function
* locate __asm__() 内嵌汇编, 能快速定位代码! 但很少! slhc_uncompress()
* 找常量!
* 找loop codes formate!
* 通过.config or CONFIG_判断具体是那个相同函数.
* 字符的数字敏感
0000000034333545 doesnt have a bit 7 set in any byte.

+ef8:   00a01021    move    v0,a1
 efc:   88440003    lwl a0,3(v0)
 f00:   24450004    addiu   a1,v0,4                                                                                                           
 f04:   98440000    lwr a0,0(v0)                                                                                                               
 f08:   00641821    addu    v1,v1,a0                                                                                                            
 f0c:   0064202b    sltu    a0,v1,a0                                                                                                             
+f10:   14a7fff9    bne a1,a3,ef8 <slhc_uncompress+0x444>                                                                                         
 f14:   00831821    addu    v1,a0,v1

### From ASM to c language x86
* local_irq_save/disable
   c: 9c                    pushfq		//not sure 
   d: 5d                    pop    %rbp //not sure
   e: fa                    cli

* per-CPU
  f: 65 48 8b 14 25 a8 d1  mov    %gs:0xd1a8,%rdx

###Track skills
####[Tips on debugging optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)
* code reordering
* inlining
* Optimized-away variables
* Tailcall optimization


### General track down case 
* If an page oops close to zero, for example 0xfffffff4
It maybe ERR_PTR(-12);

* smartqos custom qdisc - self inferrence
要自己推测除几种可能, 之后按着思路去找, 不能汪洋大海, 乱砍.

* 追BUG实际上就是, 找关联度最高的, 最好不要从头开始推理, 太耗时.


### A case of oops
Register IP: -> System.map /proc/kallsyms -> objdump -S char-read-write.ko

# Debug make bug
* Just print echo 
make -s 
* Print shell command
make -n
* Print all variables. not really execute. Wired-name variable is useful to debug
make -p
* Pirnt a message
$(warning ...)
* Etc
--warn-undefined-variables

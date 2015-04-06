---
tags: cs
title: General debugging method
date: 2015-02-27 15:46:14
category: cs
---

#BUG
drivers/net/wireless/libertas/cfg.c:1115 lbs_associate() error: potential null dereference 'cmd'.  (kzalloc returns null)
drivers/net/wireless/rtlwifi/efuse.c:378 read_efuse() error: potential null dereference 'efuse_word'.  (kmalloc returns null)
drivers/staging/rtl8192e/rtllib_rx.c:1496 rtllib_rx() error: we previously assumed 'ieee' could be null (see line 1464)
vers/media/media-device.c:146 __media_device_enum_links() warn: check that 'pad' doesn't leak information (struct has a hole after 'index')
drivers/media/media-device.c:167 __media_device_enum_links() warn: check that 'link' doesn't leak information (struct has a hole after 'index')

#[Common types of computer bugs](https://en.wikipedia.org/wiki/Software_bug#Common_types_of_computer_bugs)
## Arithmetic bugs
## syntax error
## Logic error
Incorrect Bounds-Checking
off-by-one bug
Skipping Null-Termination Issues
## Resource bugs
uninitialized/nonvalidated/corrupted pointer dereference.
Segmentation fault
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

##Taxonomy of Kernel BUG
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
1. Generating program states, 对于内核很多时候, 我们只能有一次机会产生调试的信息, 就是panic or oops那时, 不绝对.
2. examine program states 
3. track down the origin of the problem(s)
4. fix it

# Generating program states
* Printing
+ #define debugme(fmt, args...) do {FILE *fdebug=fopen("/d.log", "a+"); time_t tdebug; time(&tdebug);fprintf(fdebug,"%s", asctime(localtime(&tdebug)));fprintf(fdebug,"%s,%d:", __FUNCTION__, __LINE__);fprintf(fdebug, fmt, ##args);fclose(fdebug);} while(0)
FIXME: __TIME__

* Debugging support in the kernel
更为宽泛意义上的printing 方法.

* programming program not only printing
Use atexit() register a stackdump() function

* Trace
strace
ltrace
ftrace

* Debuger Gdb kdb
gdb /usr/src/linux/vmlinux /proc/kcore
    bt
    x/100a
thread apply all bt full

* lockdep

* tcpdump/wireshark

* kernel oops/warn/panic

* Coredump

* /proc (specially /proc/sys/)

* library dependencies of a ELF/bin
LD_TRACE_LOADED_OBJECTS=1 git 
ldd /usr/bin/git

* signal This is just a hiwifi wonderful kernel patch #931

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
objdump -s vmlinux

* Generat single sources file assemblly  codes
make kernel/sched.s V=1

## Track down
### "Wolf fence" algorithm 
二分调试大法

### From ASM to c language
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

### General track down case 
* If an page oops close to zero, for example 0xfffffff4
It maybe ERR_PTR(-12);

* Exit bug
Use atexit() register a stackdump() function
gdb,break bt

* smartqos custom qdisc - self inferrence
要自己推测除几种可能, 之后按着思路去找, 不能汪洋大海, 乱砍.


#FIXME/FIXIT

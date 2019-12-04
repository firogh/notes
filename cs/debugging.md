---
tags: [ cs ] 
title: The art of debugging
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Quote
Everyone knows that debugging is twice as hard as writing a program in the first place. So if you're as clever as you can be when you write it, how will you ever debug it?
"The Elements of Programming Style", 2nd edition, chapter 2. Brian Kernighan

# Reference
Debug hacks: a book on how to debug.

# Bug classifications
[CWE VIEW: Research Concepts](https://cwe.mitre.org/data/definitions/1000.html)
[BUG type of Jim Gray][2],除了Bohrbug,其他都不太好解.
The National Vulnerability Database是一个非常有用的网站, 国内鲜少有人提及.尤其是,
他的Bug分类方式惊艳无比, 逻辑上非常严密, 就好比数学系统, 由公理系统推导而得.
NVD的Bug分类也采用类似的构建方式.[CWE Cross Section Mapped into by NVD][3] 
涵盖了所有常见的的Bug描述, 而且非常专业.大赞!wikipedia的条目就相形见绌了
[Common types of computer bugs in wikipedia][4].

# Steps
## narrow down Hypothses 
Data: life state
call paths: related
H1, H2, H3, ... => X : But types

### Get observations
* compilation
readelf, objdump
c++filt 
* load software
LD_TRACE_LOADED_OBJECTS=1 git 
ldd /usr/bin/git
* software imediately stop
Use atexit() register a stackdump or a wrapped print

* tips
It maybe ERR_PTR(-12);
[Tips on debugging optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)
code reordering inlining Optimized-away variables Tailcall optimization

[1]: https://en.wikipedia.org/wiki/Abductive_reasoning#Logic-based_abduction
[2]: http://www.opensourceforu.com/2010/10/joy-of-programming-types-of-bugs
[3]: https://nvd.nist.gov/cwe.cfm
[4]: https://en.wikipedia.org/wiki/Software_bug#Common_types_of_computer_bugs
[5]: https://nvd.nist.gov/cwe.cfm#cweIdEntry-CWE-1

# Print
## User mode print
define witcher(fmt, args...) do{FILE * fdebug=fopen("/tmp/d.log", "a+"); \
fprintf(fdebug,"%s,%s,%d:"fmt, __TIME__, __FUNCTION__, __LINE__, ##args);fclose(fdebug);} while(0)
* make specific
-s, -n, -p, --warn-undefined-variables
$(warning ...)

# FS
lsof

# Gdb
[How debuggers work](http://eli.thegreenplace.net/2011/01/23/how-debuggers-work-part-1)
signle step instruction
## Debug glibc
directory ~/ws/glibc-2.23/libio
set solib-search-path /usr/lib/debug/lib/x86_64-linux-gnu/

# Core dump
echo /tmp/core-%e-%p-%s-%c-%P > /proc/sys/kernel/core_pattern

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

# Anti-debugging
* Syntax checking
gcc -Wall
bash -n
* static code analysis
smatch

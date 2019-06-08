---
tags: [ cs ] 
title: The art of debugging
date: 2015-02-27T15:46:14+08:00 
category: cs
---

# Reference
Debug hacks: a book on how to debug.

# OOB

# Use after free
Record the owner, who free.

# Print
## User mode print
define witcher(fmt, args...) do{FILE * fdebug=fopen("/tmp/d.log", "a+"); \
fprintf(fdebug,"%s,%s,%d:"fmt, __TIME__, __FUNCTION__, __LINE__, ##args);fclose(fdebug);} while(0)
* make specific
-s, -n, -p, --warn-undefined-variables
$(warning ...)

## User mode tracing
ltrace, strace, bash -x
[How does strace work?][1]
[1]: https://blog.packagecloud.io/eng/2016/02/29/how-does-strace-work/
PTRACE_PEEKDATA for showing pointer parameter(e.g. 2nd arg of read)

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

# Anti-debugging
* Syntax checking
gcc -Wall
bash -n
* static code analysis
smatch

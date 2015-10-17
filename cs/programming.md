---
tags: cs
title: The art of Programming
date: 2015-02-27 15:46:14
category: cs
---
# Reference
[Programming tool](https://en.wikipedia.org/wiki/Programming_tool)
[Utility software](https://en.wikipedia.org/wiki/Utility_software)
programming principles
High cohesion low coupling


#Programming style

#coding for testing

# [Programming principles](http://en.wikipedia.org/wiki/Category:Programming_principles)
## defensive programming
Low tolerance against "potential" bugs
Assertion, panic in user space

#Skills 
How to use assertion

#clean code
*all codes related to loop must in one block
__find_meaningful_task_parent_by_task()
old version: *p *pp
clean version: *tmp, *p

#Coding standards
* GNU coding standards
* Linux kernel coding style
= Shell coding standard
#Review
#Best coding practices
=Commenting
* Copyleft/right,
* Author and maintainer
* Update log
* Name of the module
* Usage
# Layers of computer
Applications
Operating system
ISA
Micro-architecture
Logical gate
Digital circuits
Analog circuits
Electronic component
Physics

# Computer programming
Programming language theory
Compiler construction
Algorithms and data structures
1.Algorithms: Design Techniques and Analysis
2.kernel/algorithm.md
Debuging
Mathmatic
1.In mathematics, and more specifically in algebra, a domain is a ring such that ab = 0 implies a = 0 or b = 0.
Testing
1.Black-box testing test software function.
2.White-box testing test software internal logic.
Automata
Design pattern
1.Publish-Subscribe Mechanism 
2.Observation
# Source code editor
* vim
easymotion

# Code navigation tools
* ctags
ctags -R --regex-c="/^SYSCALL_DEFINE[0-9]?\(([a-zA-Z0-9_]*).*/sys_\1/"
* cscope
* GNU global
* LXR Cross Referencer
* Google code search
* How to find hook functions of a struct ops
For example, all start_xmit of ppp_channel_ops, 
pppol2tp_chan_ops, pppol2tp_xmit,
pptp_chan_ops pptp_xmit
pppoe_chan_ops pppoe_xmit
or grep -nr 'ppp_channel_ops' /path/to/{l2tp, pptp, pppoe}
* locate codes in complex program source files .
Bubble locate, begin with the fundermental system api for example socket(), bind(), connect()...then raise
# Build automation
* Make
make cynthia
## kbuild
* if no .config, every config tools make a .config from scrach!
* oldconfig just for new moduels patch merged in trunk. This no any relations to .config.old and /boot/config.x.y.z
* make localmodconfig will reduce many unused kernel config.
* make bzImage  #kmods will not build that configured with M! 
* make modules_install INSTALL_MOD_PATH=/home/firo/kmods
make ARCH=arm menuconfig
##build signle kernel module
make menuconfig
make oldconfig && make prepare
make -C $(pwd) M=/home/firo/linux/fs/ext3 modules V=1
make ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnu- drivers/pcmcia/sa11xx_base.o
# Compile
* gcc
-E -S -c 
-I -L -l
-ansi
-fsyntax-only 
-Werror
-save-temps
-H: show all header used
-v
-Wall
-fPIC: for shared library
-Q: print function and statisc?
-Wextra: 用处不大.
-U: undefine a preprocessor macro.
-pipe: make compilation faster, replace xx.S with |
-Wpadded: we can rearrange the fields of structure to make the structure smaller
-time
-x c: c language
-: stdin
asmlinkage
However, for C functions invoked from assembly code, 
we should explicitly declare the function's calling convention, 
because the parameter passing code in assembly side has been fixed. 
##[Generating optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)
##Symbol table
* System.map less 
When you compile the kernel
nm vmlinux
readelf -s 
* /proc/kallsyms
# Link
* GNU ld
* gold
# Version control
* [git](http://git-scm.com/docs)
git ls-files -d |xargs -i git checkout {}
git log -S "RTN_UNSPEC" --source --all
* reverse find all the blame from start commit id to now
git blame --reverse 9e734775f7c22d2f89943ad6c745571f1930105f..   include/linux/netlink.h
* find kernel version of path
git describe --contains
git tag --contains <id>
* find commit id of a tag
git rev-parse v2.6.12-rc2
*  blame before a commit
git blame sha1^ -- path/to/file
* find commit after specific commit
git log  --oneline  --ancestry-path   c146066ab^^..HEAD  net/ipv4/ip_output.c
* find when line was deleted
git log -c -S'missingtext' /path/to/file
## git pull
git pull {repo} {remotebranchname}:{localbranchname}
git pull origin xyz:xyz
git pull next master:now
## git sendmail
shadowsocks
/etc/proxychains.conf
/home/firo/.gitconfig
proxychains git send-email --to dbaryshkov@gmail.com --cc rmk+kernel@arm.linux.org.uk --cc linux-pcmcia@lists.infradead.org  0001-Drivers-pcmcia-Fix-memroy-leak-in-sa11xx_drv_pcmcia_.patch
git send-email --smtp-encryption=tls --smtp-server=smtp.gmail.com --smtp-user=firogm@gmail.com --smtp-server-port=587 --to "kernel-janitors@vger.kernel.org" 0001-x86-Remove-unnecessary-void-pointer-casts.patch
##git diff
git -c core.whitespace=tab-in-indent diff --check
# Debugging
see debugging.md
# Test
* kvm  & supermin
# Mail
mutt
fetchmail
fetchmail -d0 -vk pop.gmail.com
msmtp
/home/firo/.msmtprc
procmail


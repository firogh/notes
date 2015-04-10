---
tags: kernel
title: The linux development environment
date: 2015-02-27 15:46:12
category: kernel
---
# Reference
[Programming tool](https://en.wikipedia.org/wiki/Programming_tool)
[Utility software](https://en.wikipedia.org/wiki/Utility_software)

# Development environment
## Source code editor
* vim

## Code navigation tools
* ctags
ctags -R --regex-c="/^SYSCALL_DEFINE[0-9]?\(([a-zA-Z0-9_]*).*/sys_\1/"

* cscope
* GNU global
* LXR Cross Referencer
* Google code search

## Revision control
* [git](http://git-scm.com/docs)
* diff patch

## Build

### Build automation
* Make

###Compiler
* gcc

### Linker
* GNU ld
* gold


## Reverse engineering
* readelf
* objdump
* tcpdump

## Test

## Debug
More details in debugging note
#Git 
git send-email --smtp-encryption=tls --smtp-server=smtp.gmail.com --smtp-user=firogm@gmail.com --smtp-server-port=587 --to "kernel-janitors@vger.kernel.org" 0001-x86-Remove-unnecessary-void-pointer-casts.patch

git ls-files -d |xargs -i git checkout {}

git log -S "RTN_UNSPEC" --source --all
* reverse find all the blame from start commit id to now
    git blame --reverse 9e734775f7c22d2f89943ad6c745571f1930105f..   include/linux/netlink.h

* find kernel version of path
    git describe --contains
    git tag --contains <id>

* find commit id of a tag
    git rev-parse v2.6.12-rc2


#Make
##debug
* Just print echo 
make -s 
* Print shell command
make -n
* Print all variables. Wired-name variable is useful to debug
make -p
* Pirnt a message
$(warning ...)
* Etc
--warn-undefined-variables

##menuconfig
* if no .config, every config tools make a .config from scrach!

* oldconfig just for new moduels patch merged in trunk. This no any relations to .config.old and /boot/config.x.y.z

* make localmodconfig will reduce many unused kernel config.

* make bzImage  #kmods will not build that configured with M! 
* make modules_install INSTALL_MOD_PATH=/home/firo/kmods

##build signle kernel module
make menuconfig
make oldconfig && make prepare
make -C $(pwd) M=/home/firo/linux/fs/ext3 modules V=1


#GCC
##[Generating optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)

##Symbol table
* System.map less 
When you compile the kernel
nm vmlinux
* /proc/kallsyms

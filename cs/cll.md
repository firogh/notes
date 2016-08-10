---
tags: [ cs ] 
title: Light camera action - compile link load 
date: 2015-10-25T01:46:28+08:00 
category: cs
---
# Reference
[compiler, assembler, linker and loader: a brief story](http://www.tenouk.com/ModuleW.html)
[Linkers 20 parts by Gold author Ian Lance Taylor](http://a3f.at/lists/linkers)
[通过 GDB 调试理解 GOT/PLT](http://rickgray.me/2015/08/07/use-gdb-to-study-got-and-plt.html)
[PLT and GOT - the key to code sharing and dynamic libraries](https://www.technovelty.org/linux/plt-and-got-the-key-to-code-sharing-and-dynamic-libraries.html)

# ELF
[System V Application Binary Interface ELF](http://www.sco.com/developers/gabi/latest/contents.html)
[SCO Developer Specifications](http://www.sco.com/developers/devspecs/)

# Function call
x86: stack and caller but kernel maybe regpara(3)

create_elf_tables
sysdeps/x86_64/dl-machine.h: -> _start ->_dl_start->/elf/dl-sysdep.c:83:_dl_sysdep_start->dl_main
Relocation
PIC

# Contents
Why complier can manipulate the address of vriables? 
What is the address in lds(??) file?

## kbuild
* if no .config, every config tools make a .config from scrach!
* oldconfig just for new moduels patch merged in trunk. This no any relations to .config.old and /boot/config.x.y.z
* make localmodconfig will reduce many unused kernel config.
* make bzImage  #kmods will not build that configured with M! 
* make modules_install INSTALL_MOD_PATH=/home/firo/kmods
##build signle kernel module
make menuconfig
make oldconfig && make prepare
make -C $(pwd) M=/home/firo/linux/fs/ext3 modules V=1
make ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnu- drivers/pcmcia/sa11xx_base.o
# Compile construction
##[Generating optimized code](http://www.stlinux.com/devel/debug/jtag/build?q=node/82)
##Symbol table
* System.map less 
When you compile the kernel
nm vmlinux
readelf -s 
* /proc/kallsyms
